//
//  AddBuddy.cpp
//  ETImServer
//
//  Created by Ethan on 14/7/30.
//  Copyright (c) 2014年 Pingan. All rights reserved.
//

#include "AddBuddy.h"
#include "InStream.h"
#include "OutStream.h"
#include "MD5.h"
#include "Idea.h"
#include "DataStruct.h"

#include <algorithm>

using namespace etim;
using namespace etim::action;
using namespace::etim::pub;
using namespace std;

void RequestAddBuddy::DoSend(Session& s) {
    OutStream jos;
    
	// 包头命令
	uint16 cmd = CMD_REQUEST_ADD_BUDDY;
	jos<<cmd;
    
	// 预留两个字节包头len（包体+包尾长度）
	size_t lengthPos = jos.Length();
	jos.Skip(2);
    
	// 要查找的用户名
	string fromName = s.GetAttribute("friend_from");
    string toName = s.GetAttribute("friend_to");
    transform(fromName.begin(), fromName.end(), fromName.begin(), ::tolower);
    transform(toName.begin(), toName.end(), toName.begin(), ::tolower);
	jos<<fromName;
    jos<<toName;
    
    FillOutPackage(jos, lengthPos, cmd);
    
	s.Send(jos.Data(), jos.Length());	// 发送请求包
}

void RequestAddBuddy::DoRecv(etim::Session &s) {
    InStream jis((const char*)s.GetResponsePack(), s.GetResponsePack()->head.len+sizeof(ResponseHead));
	// 跳过cmd、len
	jis.Skip(4);
	uint16 cnt;
	uint16 seq;
	int16 error_code;
	jis>>cnt>>seq>>error_code;
    
	char error_msg[ERR_MSG_LENGTH+1];
	jis.ReadBytes(error_msg, ERR_MSG_LENGTH);
    
 	s.SetErrorCode(error_code);
	s.SetErrorMsg(error_msg);
}


void SearchBuddy::DoSend(Session &s) {
    OutStream jos;
    
	// 包头命令
	uint16 cmd = CMD_SEARCH_BUDDY;
	jos<<cmd;
    
	// 预留两个字节包头len（包体+包尾长度）
	size_t lengthPos = jos.Length();
	jos.Skip(2);
    
	// 要查找的用户名
	string name = s.GetAttribute("name");
    transform(name.begin(),name.end(), name.begin(), ::tolower);
	jos<<name;

	FillOutPackage(jos, lengthPos, cmd);
    
	s.Send(jos.Data(), jos.Length());	// 发送请求包
}

void SearchBuddy::DoRecv(etim::Session &s) {
    InStream jis((const char*)s.GetResponsePack(), s.GetResponsePack()->head.len+sizeof(ResponseHead));
	// 跳过cmd、len
	jis.Skip(4);
	uint16 cnt;
	uint16 seq;
	int16 error_code;
	jis>>cnt>>seq>>error_code;
    
	char error_msg[ERR_MSG_LENGTH+1];
	jis.ReadBytes(error_msg, ERR_MSG_LENGTH);
    
    if (error_code == kErrCode00) {
        IMUser user;
        int rel;
        int status;
        jis>>user.userId;
        jis>>user.username;
        jis>>user.regDate;
        jis>>user.signature;
        jis>>user.gender;
        jis>>rel;
        jis>>status;
        jis>>user.statusName;
        user.relation = static_cast<BuddyRelation>(rel);
        user.status = static_cast<BuddyStatus>(status);
        
        s.SetSearchIMUser(user);
    }
    s.SetErrorCode(error_code);
	s.SetErrorMsg(error_msg);
}

void AcceptAddBuddy::DoSend(Session &s) {
    OutStream jos;
    
	// 包头命令
	uint16 cmd = CMD_ACCEPT_ADD_BUDDY;
	jos<<cmd;
    
	// 预留两个字节包头len（包体+包尾长度）
	size_t lengthPos = jos.Length();
	jos.Skip(2);
    
	// 要查找的用户名
    string reqId = s.GetAttribute("reqId");
	string fromId = s.GetAttribute("fromId");
    string addPeer = s.GetAttribute("addPeer");
	jos<<reqId;
    jos<<fromId;
    jos<<addPeer;
    
	FillOutPackage(jos, lengthPos, cmd);
    
	s.Send(jos.Data(), jos.Length());	// 发送请求包
}

void AcceptAddBuddy::DoRecv(etim::Session &s) {
    InStream jis((const char*)s.GetResponsePack(), s.GetResponsePack()->head.len+sizeof(ResponseHead));
	// 跳过cmd、len
	jis.Skip(4);
	uint16 cnt;
	uint16 seq;
	int16 error_code;
	jis>>cnt>>seq>>error_code;
    
	char error_msg[ERR_MSG_LENGTH+1];
	jis.ReadBytes(error_msg, ERR_MSG_LENGTH);
    if (error_code == kErrCode00) {
        IMUser user;
        int rel;
        int status;
        string addPeer;
        jis>>user.userId;
        jis>>user.username;
        jis>>user.regDate;
        jis>>user.signature;
        jis>>user.gender;
        jis>>rel;
        jis>>status;
        jis>>user.statusName;
        jis>>addPeer;
        user.relation = static_cast<BuddyRelation>(rel);
        user.status = static_cast<BuddyStatus>(status);
        //TODO 服务器返回接收成功对方可以添加我为好友如果有添加对方 那么我要将此USER添加进我的好友列表
        s.ClearBuddys();
        if (Convert::StringToInt(addPeer)) {
            s.AddBuddy(user);
        }
    }
   	s.SetErrorCode(error_code);
	s.SetErrorMsg(error_msg);
}

void RejectAddBuddy::DoSend(Session &s) {
    OutStream jos;
    
	// 包头命令
	uint16 cmd = CMD_REJECT_ADD_BUDDY;
	jos<<cmd;
    
	// 预留两个字节包头len（包体+包尾长度）
	size_t lengthPos = jos.Length();
	jos.Skip(2);
    
	// req及对方信息
    string reqId = s.GetAttribute("reqId");
	string fromId = s.GetAttribute("fromId");
	jos<<reqId;
    jos<<fromId;
    
	FillOutPackage(jos, lengthPos, cmd);
    
	s.Send(jos.Data(), jos.Length());	// 发送请求包
}

void RejectAddBuddy::DoRecv(etim::Session &s) {
    InStream jis((const char*)s.GetResponsePack(), s.GetResponsePack()->head.len+sizeof(ResponseHead));
	// 跳过cmd、len
	jis.Skip(4);
	uint16 cnt;
	uint16 seq;
	int16 error_code;
	jis>>cnt>>seq>>error_code;
    
	char error_msg[ERR_MSG_LENGTH+1];
	jis.ReadBytes(error_msg, ERR_MSG_LENGTH);
    
	s.SetErrorCode(error_code);
	s.SetErrorMsg(error_msg);
}