#include "sema.hpp"

namespace compiler
{

void Sema::add(ast::FunctionDecl const&) 
{ 
    
}

void Sema::add(ast::ObjDecl const&)
{
    // obj.type();
    // obj.iden();
    //
    // if (obj.init() != nullptr)
    // {
    //     // TODO        
    // }
}


void Sema::push() 
{

}

void Sema::pop() 
{

}

} // namespace compiler
