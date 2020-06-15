#include "serializer.hpp"


string to_string(const Context &context)
{
    PtreeOArchive ptree_arch;
    Context context2 = context; // bad: deep copy
    context2.serialize<PtreeOArchive>(ptree_arch, 0);
    return ptree_arch.to_json_string();
}

Context context_from_string(const string &text)
{
    PtreeIArchive ptree_arch(text);
    Context context;
    context.serialize<PtreeIArchive>(ptree_arch, 0);
    return context;
}
