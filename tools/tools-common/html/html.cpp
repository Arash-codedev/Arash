#include "html.hpp"
#include <libs/easytext.hpp>



NS_LANGUAGE_BEGIN

string header_script(const string & link)
{
    std::ostringstream stream;
    stream << "<script src=\"" << link << "\"></script>" << endl;
    return stream.str();
}

string header_css(const string & link)
{
    std::ostringstream stream;
    stream << "<link href=\"" << link << "\" rel=\"stylesheet\">" << endl;
    return stream.str();
}

// string general_styles()
// {
//     std::ostringstream stream;

//     stream << "body {" << endl;
//     stream << "   height: 100%;" << endl;
//     stream << "   padding: 0;" << endl;
//     stream << "   margin: 0;" << endl;
//     stream << "}" << endl;
//     stream << "" << endl;
//     stream << ".page-content {" << endl;
//     stream << "    display: -webkit-flex;" << endl;
//     stream << "    display: flex;" << endl;
//     // stream << "    height: 100vh;" << endl;
//     stream << "}" << endl;
//     stream << "" << endl;
//     stream << ".nav-pane {" << endl;
//     stream << "  -webkit-flex: 1;" << endl;
//     stream << "  -ms-flex: 1;" << endl;
//     stream << "  flex: 1;" << endl;
//     stream << "  background: #263050;" << endl;
//     stream << "  padding: 20px;" << endl;
//     stream << "}" << endl;
//     stream << "" << endl;
//     stream << "nav ul li a {" << endl;
//     stream << "  color: #f1f1f1;" << endl;
//     stream << "}" << endl;
//     stream << "" << endl;
//     stream << "nav{" << endl;
//     stream << "  color: #f1f1f1;" << endl;
//     stream << "}" << endl;
//     stream << "" << endl;
//     stream << ".content-pane {" << endl;
//     stream << "  -webkit-flex: 4;" << endl;
//     stream << "  -ms-flex: 4;" << endl;
//     stream << "  flex: 4;" << endl;
//     stream << "  background-color: #f9fbfd;" << endl;
//     stream << "  padding: 10px;" << endl;
//     stream << "}" << endl;
//     stream << "" << endl;
//     stream << "nav ul {" << endl;
//     stream << "  list-style-type: none;" << endl;
//     stream << "  padding: 0;" << endl;
//     stream << "}" << endl;
//     stream << "" << endl;
//     stream << ".source-code {" << endl;
//     stream << "  background-color: #fffdbf;" << endl;
//     stream << "}" << endl;
//     stream << "" << endl;
//     // stream << ".splitter {" << endl;
//     // stream << "  flex: none;" << endl;
//     // stream << "  width: 17px;" << endl;
//     // stream << "  cursor: col-resize;" << endl;
//     // stream << "  background: #535353;" << endl;
//     // stream << "  background: url(https://raw.githubusercontent.com/RickStrahl/jquery-resizable/master/assets/vsizegrip.png) center center no-repeat #535353;" << endl;
//     // stream << "}" << endl;
//     stream << "" << endl;
//     return stream.str();
// }

// string general_script()
// {
//     std::ostringstream stream;
//     stream << "$(function() {" << endl;
//     stream << "  // Handler for .ready() called." << endl;
//     // stream << "  $(\".sidebar-left\").resizable({" << endl;
//     // stream << "     handleSelector: \".splitter\"," << endl;
//     // stream << "     resizeHeight: false" << endl;
//     // stream << "   });" << endl;
//     stream << "});" << endl;
//     return stream.str();
// }

string wrap_body(const string & body_content, const string & title)
{
    std::ostringstream html_stream;
    html_stream << "<!DOCTYPE html>" << endl;
    html_stream << "<html>" << endl;
    html_stream << "<meta charset=\"UTF-8\">" << endl;
    // html_stream << header_script("https://code.jquery.com/jquery-3.5.1.js") << endl;
    // html_stream << header_script("https://code.jquery.com/ui/1.12.1/jquery-ui.js") << endl;
    html_stream << header_script("html_resources/jquery-3.5.1.js") << endl;
    html_stream << header_script("html_resources/jquery-ui.js") << endl;
    html_stream << header_script("html_resources/main.js") << endl;
    html_stream << header_css("html_resources/main.css") << endl;
    html_stream << "<title>" << title << "</title>" << endl;
    // html_stream << "<style>" << endl;
    // html_stream << general_styles() << endl;
    // html_stream << "</style>" << endl;
    html_stream << "<body>" << endl;
    html_stream << body_content<< endl;
    // html_stream << "<script>" << endl;
    // html_stream << general_script() << endl;
    // html_stream << "</script>" << endl;
    html_stream << "</body>" << endl;
    html_stream << "</html>" << endl;
    return html_stream.str();
}

string h2(const string & content)
{
    return "<h2>" + content + "</h2>";
}

string br()
{
    return "<br>";
}

string escape(const string & html_content)
{
    string result = html_content;
    result = easytext::replace_all(result, "&", "&amp;"); // should be first
    result = easytext::replace_all(result, "<", "&lt;");
    result = easytext::replace_all(result, ">", "&gt;");
    result = easytext::replace_all(result, "\'", "&quot;");
    return result;
}

string preserve_space(const string & html_content)
{
    string result = html_content;
    result = easytext::replace_all(result, "  ", "&nbsp;&nbsp;");
    return result;
}

NS_LANGUAGE_END
