
//
//  Login.cpp
//  ETImServer
//
//  Created by Ethan on 14/7/30.
//  Copyright (c) 2014年 Pingan. All rights reserved.
//

#include "Login.h"
#include "OutStream.h"
#include "InStream.h"
#include "MD5.h"
#include "Idea.h"
#include "glog/logging.h"
#include "DataService.h"
#include "DataStruct.h"
#include "PushService.h"

#include <sstream>
#include <iomanip>
#include <sys/time.h>
#include <time.h>

using namespace etim;
using namespace etim::pub;
using namespace etim::action;
using namespace std;

/**
 用户登录
 */
void Login::Execute(Session *s) {
    InStream jis(s->GetRequestPack()->buf, s->GetRequestPack()->head.len);
	uint16 cmd = s->GetCmd();
    
	// 登录名
	string name;
	jis>>name;
	// 密码
	char pass[16];
	unsigned char ideaKey[16];
	unsigned char buf[2];
	buf[0] = (cmd >> 8) & 0xff;
	buf[1] = cmd & 0xff;
	MD5 md5;
	md5.MD5Make(ideaKey, buf, 2);
	for (int i=0; i<8; ++i) {
		ideaKey[i] = ideaKey[i] ^ ideaKey[i+8];
		ideaKey[i] = ideaKey[i] ^ ((cmd >> (i%2)) & 0xff);
		ideaKey[i+8] = ideaKey[i] ^ ideaKey[i+8];
		ideaKey[i+8] = ideaKey[i+8] ^ ((cmd >> (i%2)) & 0xff);
	}
	char encryptedPass[16];
	jis.ReadBytes(encryptedPass, 16);
	Idea idea;
	// 解密
	idea.Crypt(ideaKey, (const unsigned char*)encryptedPass, (unsigned char *)pass, 16, false);
    
	int16 error_code = kErrCode00;
	char error_msg[ERR_MSG_LENGTH+1] = {0};
    
	// 实际的登录操作
    //TODO 用户已登录判断
	DataService dao;
    IMUser user;
    user.username = name;
	int ret;
	ret = dao.UserLogin(name, pass, user);
	if (ret == kErrCode00) {
        s->SetIMUser(user);
		LOG(INFO)<<"登录成功: "<<name;
	} else  {
		error_code = ret;
        assert(error_code < kErrCodeMax);
		strcpy(error_msg, gErrMsg[error_code].c_str());
		LOG(INFO)<<"登录出错: "<<error_msg <<"name: "<<name;
	}
    
	OutStream jos;
	// 包头命令
	jos<<cmd;
	size_t lengthPos = jos.Length();
	jos.Skip(2);///留出2个字节空间用来后面存储包体长度+包尾(8)的长度
	// 包头cnt、seq、error_code、error_msg
	uint16 cnt = 0;
	uint16 seq = 0;
	jos<<cnt<<seq<<error_code;
	jos.WriteBytes(error_msg, ERR_MSG_LENGTH);
    
	// 包体
    if (ret == kErrCode00) {
        jos<<user.userId;
        jos<<user.username;
        jos<<user.regDate;
        jos<<user.signature;
        jos<<user.gender;
        jos<<user.relation;
        jos<<user.status;
        jos<<user.statusName;
    }
    
    FillOutPackage(jos, lengthPos, cmd);
     
	s->Send(jos.Data(), jos.Length());
    
    if (ret == kErrCode00) {
        PushService push;
        push.PushBuddyUpdate(user, dao);
    }
}

/**
 用户登出
 */
void Logout::Execute(Session *s) {
    InStream jis(s->GetRequestPack()->buf, s->GetRequestPack()->head.len);
	uint16 cmd = s->GetCmd();
    
	// 登录名
	string userId;
	jis>>userId;
	
    int16 error_code = kErrCode00;
	char error_msg[ERR_MSG_LENGTH+1] = {0};
	DataService dao;
    IMUser user;
	int ret = kErrCode00;
    ret = dao.UserLogout(userId, user); ///数据库已更新为offline
	if (ret == kErrCode00) {
		LOG(INFO)<<"登出成功 userId: "<<userId;
        s->SetIMUser(user);
	} else  {
		error_code = ret;
        assert(error_code < kErrCodeMax);
		strcpy(error_msg, gErrMsg[error_code].c_str());
		LOG(ERROR)<<"登出出错: "<<error_msg<<" userId: "<<userId;
	}
    
	OutStream jos;
	// 包头命令
	jos<<cmd;
	size_t lengthPos = jos.Length();
	jos.Skip(2);///留出2个字节空间用来后面存储包体长度+包尾(8)的长度
	// 包头cnt、seq、error_code、error_msg
	uint16 cnt = 0;
	uint16 seq = 0;
	jos<<cnt<<seq<<error_code;
	jos.WriteBytes(error_msg, ERR_MSG_LENGTH);
    
	// 空包体

    FillOutPackage(jos, lengthPos, cmd);
	s->Send(jos.Data(), jos.Length());
    
    if (ret == kErrCode00) {
        PushService push;
        push.PushBuddyUpdate(user, dao);
    }
    
    //删除此会话通过KICKOUT删除 如果此时删除 会影响正在循环的server中的vector<Session *> sessions_;
    //同时也会抛出recv 0异常在SERVER中删除
    /*
    timeval now;
    gettimeofday(&now, NULL);
    now.tv_sec = now.tv_sec - HEART_BEAT_SECONDS * 30;
    s->SetLastTime(now);
     */
}

