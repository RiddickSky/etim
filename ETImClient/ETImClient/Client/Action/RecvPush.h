//
//  RecvPush.h
//  ETImClient
//
//  Created by Ethan on 14/9/10.
//  Copyright (c) 2014年 Pingan. All rights reserved.
//

#ifndef __ETImClient__RecvPush__
#define __ETImClient__RecvPush__

#include <iostream>
#include "Action.h"

namespace etim {
    namespace action {
        

        ///接受用户状态更新
        class PushBuddyUpdate : public Action {
        public:
            virtual void DoSend(Session& s);
            virtual void DoRecv(Session &s);
        };
        
        ///接受好友请求更新
        class PushBuddyRequest : public Action {
        public:
            virtual void DoSend(Session& s);
            virtual void DoRecv(Session &s);
        };
        
        class PushRequestAddBuddy : public Action {
        public:
            virtual void DoSend(Session& s);
            virtual void DoRecv(Session &s);
        };
    } //end action
} //end etim

#endif /* defined(__ETImClient__RecvPush__) */
