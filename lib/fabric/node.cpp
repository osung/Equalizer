
/* Copyright (c) 2010, Stefan Eilemann <eile@eyescale.ch>
 *               2010, Cedric Stalder <cedric.stalder@gmail.com>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "node.h"

#include "elementVisitor.h"
#include "leafVisitor.h"
#include "paths.h"

namespace eq
{
namespace fabric
{
namespace
{
#define MAKE_ATTR_STRING( attr ) ( std::string("EQ_NODE_") + #attr )
std::string _iAttributeStrings[] = {
    MAKE_ATTR_STRING( IATTR_THREAD_MODEL ),
    MAKE_ATTR_STRING( IATTR_LAUNCH_TIMEOUT ),
    MAKE_ATTR_STRING( IATTR_FILL1 ),
    MAKE_ATTR_STRING( IATTR_FILL2 )
};
}

template< class C, class N, class P, class V >
Node< C, N, P, V >::Node( C* parent )  
        : _config( parent )
{
    parent->_addNode( static_cast< N* >( this ) );
}

template< class C, class N, class P, class V >
Node< C, N, P, V >::~Node()  
{
    while( !_pipes.empty() )
    {
        P* pipe = _pipes.back();
        _removePipe( pipe );
        delete pipe;
    }

    _config->_removeNode( static_cast< N* >( this ) );
}

namespace
{
template< class N, class V >
VisitorResult _accept( N* node, V& visitor )
{
    VisitorResult result = visitor.visitPre( node );
    if( result != TRAVERSE_CONTINUE )
        return result;

    const typename N::PipeVector& pipes = node->getPipes();
    for( typename N::PipeVector::const_iterator i = pipes.begin(); 
         i != pipes.end(); ++i )
    {
        switch( (*i)->accept( visitor ))
        {
            case TRAVERSE_TERMINATE:
                return TRAVERSE_TERMINATE;

            case TRAVERSE_PRUNE:
                result = TRAVERSE_PRUNE;
                break;
                
            case TRAVERSE_CONTINUE:
            default:
                break;
        }
    }

    switch( visitor.visitPost( node ))
    {
        case TRAVERSE_TERMINATE:
            return TRAVERSE_TERMINATE;

        case TRAVERSE_PRUNE:
            return TRAVERSE_PRUNE;
                
        case TRAVERSE_CONTINUE:
        default:
            break;
    }

    return result;
}
}

template< class C, class N, class P, class V >
VisitorResult Node< C, N, P, V >::accept( V& visitor )
{
    return _accept( static_cast< N* >( this ), visitor );
}

template< class C, class N, class P, class V >
VisitorResult Node< C, N, P, V >::accept( V& visitor ) const
{
    return _accept( static_cast< const N* >( this ), visitor );
}

template< class C, class N, class P, class V >
NodePath Node< C, N, P, V >::getPath() const
{
    const C* config = static_cast< const N* >( this )->getConfig( );
    EQASSERT( config );
    
    const typename std::vector< N* >& nodes = config->getNodes();
    typename std::vector< N* >::const_iterator i = std::find( nodes.begin(),
                                                              nodes.end(),
                                                              this );
    EQASSERT( i != nodes.end( ));

    NodePath path;
    path.nodeIndex = std::distance( nodes.begin(), i );
    return path;
}

template< class C, class N, class P, class V >
void Node< C, N, P, V >::setIAttribute( const IAttribute attr, const int32_t value )
{
    _iAttributes[attr] = value;
}

template< class C, class N, class P, class V >
int32_t Node< C, N, P, V >::getIAttribute( const IAttribute attr ) const
{
    return _iAttributes[attr];
}

template< class C, class N, class P, class V >
const std::string& Node< C, N, P, V >::getIAttributeString( const IAttribute attr )
{
    return _iAttributeStrings[attr];
}

template< class C, class N, class P, class V >
void Node< C, N, P, V >::_addPipe( P* pipe )
{
    EQASSERT( pipe->getNode() == this );
    _pipes.push_back( pipe );
}

template< class C, class N, class P, class V >
bool Node< C, N, P, V >::_removePipe( P* pipe )
{
    typename PipeVector::iterator i = stde::find( _pipes, pipe );
    if( i == _pipes.end( ))
        return false;

    _pipes.erase( i );
    return true;
}

template< class C, class N, class P, class V >
P* Node< C, N, P, V >::findPipe( const uint32_t id )
{
    for( typename PipeVector::const_iterator i = _pipes.begin();
         i != _pipes.end(); ++i )
    {
        P* pipe = *i;
        if( pipe->getID() == id )
            return pipe;
    }
    return 0;
}

}
}
