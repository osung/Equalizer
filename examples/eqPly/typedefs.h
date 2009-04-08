/*  
    typedefs.h
    Copyright (c) 2007, Tobias Wolf <twolf@access.unizh.ch>
  *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
  
    
    Type definitions for the mesh classes.
*/


#ifndef MESH_TYPEDEFS_H
#define MESH_TYPEDEFS_H

#define EQUALIZER  1

#ifdef EQUALIZER
#   include <eq/eq.h>
#   define MESHASSERT  EQASSERT
#   define MESHERROR   EQERROR
#   define MESHWARN    EQWARN
#   define MESHINFO    EQINFO
#else
#   ifdef _WIN32
#      include <Winsock2.h>
#      include <Windows.h>
#   endif
#   ifdef __APPLE__
#      include <OpenGL/gl.h>
#   else
#      include <GL/gl.h>
#   endif
#   include <cassert>
#   define MESHASSERT  assert
#   define MESHERROR   std::cerr
#   define MESHWARN    std::cout
#   define MESHINFO    std::cout
#endif

#include <vmmlib/vmmlib.h>

#include <exception>
#include <iostream>
#include <string>

namespace mesh 
{
    
    
    // basic type definitions   
    typedef vmml::Vector3< GLfloat >    Vertex;
    typedef vmml::Vector4< GLubyte >    Color;
    typedef vmml::Vector3< GLfloat >    Normal;
    typedef size_t                      Index;
    typedef GLushort                    ShortIndex;
    
    
    // mesh exception
    struct MeshException : public std::exception
    {
        explicit MeshException( const std::string& msg ) : _message( msg ) {}
        virtual ~MeshException() throw() {}
        virtual const char* what() const throw() { return _message.c_str(); }
    private:
        std::string _message;
    };
    
    // null output stream that discards everything written to it
    struct NullOStream : std::ostream
    {
        struct NullStreamBuf : std::streambuf
        {
            int overflow( int c ) { return traits_type::not_eof( c ); }
        } _nullBuf;
        
        NullOStream() : std::ios( &_nullBuf ), std::ostream( &_nullBuf ) {}
    };
    
    // wrapper to enable array use where arrays would not be allowed otherwise
    template< class T, size_t d >
    struct ArrayWrapper
    {
        T& operator[]( const size_t i )
        {
            MESHASSERT( i < d );
            return data[i];
        }
        
        const T& operator[]( const size_t i ) const
        {
            MESHASSERT( i < d );
            return data[i];
        }
        
    private:
        T data[d];
    };
    
    
    // compound type definitions
    typedef vmml::Vector3< Index >      Triangle;
    typedef ArrayWrapper< Vertex, 2 >   BoundingBox;
    typedef vmml::Vector4< float >      BoundingSphere;
    typedef ArrayWrapper< float, 2 >    Range;
    
    
    // maximum triangle count per leaf node (keep in mind that the number of
    // different vertices per leaf must stay below ShortIndex range; usually
    // #vertices ~ #triangles/2, but max #vertices = #triangles * 3)
    const Index             LEAF_SIZE( 21845 );
    
    // binary mesh file version, increment if changing the file format
    const unsigned short    FILE_VERSION ( 0x0115 );
    
    
    // enumeration for the sort axis
    enum Axis
    {
        AXIS_X,
        AXIS_Y,
        AXIS_Z
    };
    inline std::ostream& operator << ( std::ostream& os, const Axis axis )
    {
        os << ( axis == AXIS_X ? "x axis" : axis == AXIS_Y ? "y axis" :
                axis == AXIS_Z ? "z axis" : "ERROR" );
        return os;
    }
    
    // enumeration for the buffer objects
    enum BufferObject
    {
        VERTEX_OBJECT,
        NORMAL_OBJECT,
        COLOR_OBJECT,
        INDEX_OBJECT
    };
    
    // enumeration for the render modes
    enum RenderMode
    {
        RENDER_MODE_IMMEDIATE = 0,
        RENDER_MODE_DISPLAY_LIST,
        RENDER_MODE_BUFFER_OBJECT,
        RENDER_MODE_ALL // must be last
    };
    inline std::ostream& operator << ( std::ostream& os, const RenderMode mode )
    {
        os << ( mode == RENDER_MODE_IMMEDIATE     ? "immediate mode" : 
                mode == RENDER_MODE_DISPLAY_LIST  ? "display list mode" : 
                mode == RENDER_MODE_BUFFER_OBJECT ? "VBO mode" : "ERROR" );
        return os;
    }
    
    // enumeration for kd-tree node types
    enum NodeType
    {
        ROOT_TYPE = 0x07,
        NODE_TYPE = 0xde,
        LEAF_TYPE = 0xef
    };
    
    
    // helper function for MMF (memory mapped file) reading
    inline
    void memRead( char* destination, char** source, size_t length )
    {
        memcpy( destination, *source, length );
        *source += length;
    }
    
    
    // internally linked null stream, every translation unit gets a copy
    static mesh::NullOStream cnul;
}


#endif // MESH_TYPEDEFS_H
