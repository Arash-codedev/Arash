#pragma once
#include <00_common/protocol.hpp>
#include <light_header>



#define NS_LANGUAGE_BEGIN namespace html{
#define NS_LANGUAGE_END }

NS_LANGUAGE_BEGIN

string wrap_body(const string & body_content, const string & title);
string h2(const string & content);
string br();
string escape(const string & html_content);
string preserve_space(const string & html_content);

NS_LANGUAGE_END
