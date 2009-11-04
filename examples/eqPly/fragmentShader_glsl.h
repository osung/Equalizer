//Generated file - Edit fragmentShader.glsl!
static const std::string fragmentShader_glsl = "/*  \n  *  fragmentShader.glsl\n  *  Copyright (c) 2007, Tobias Wolf <twolf@access.unizh.ch>\n  *\n  * This library is free software; you can redistribute it and/or modify it under\n  * the terms of the GNU Lesser General Public License version 2.1 as published\n  * by the Free Software Foundation.\n  *  \n  * This library is distributed in the hope that it will be useful, but WITHOUT\n  * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n  * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more\n  * details.\n  * \n  * You should have received a copy of the GNU Lesser General Public License\n  * along with this library; if not, write to the Free Software Foundation, Inc.,\n  * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.\n  */ \n     \n // Fragment shader for Phong/Blinn-Phong Shading with one light source.\n \n \n varying vec3 normalEye;\n varying vec4 positionEye;\n \n \n void main()\n {\n     // normalize interpolated normal, compute view vector from position\n     vec3 normal = normalize( normalEye );\n     vec3 view = normalize( -positionEye ).xyz;\n     \n     // compute light vector\n     vec3 light;\n     if( gl_LightSource[0].position.w == 0.0 )\n         // directional light\n         light = normalize( gl_LightSource[0].position ).xyz;\n     else\n         // point light\n         light = normalize( gl_LightSource[0].position - positionEye ).xyz;\n     \n     // compute the ambient component\n     //vec4 ambient = gl_FrontLightProduct[0].ambient;\n     vec4 ambient = gl_LightSource[0].ambient * gl_Color;\n     \n     // compute the diffuse component\n     float dotLN = dot( light, normal );\n     //vec4 diffuse = gl_FrontLightProduct[0].diffuse * max( dotLN, 0.0 );\n     vec4 diffuse = gl_LightSource[0].diffuse * gl_Color * max( dotLN, 0.0 );\n     \n     // compute the specular component\n     float factor;\n     if( dotLN > 0.0 )\n         factor = 1.0;\n     else\n         factor = 0.0;\n     \n     // pure Phong\n     //vec3 reflect = normalize( reflect( -light, normal ) );\n     //vec4 specular = \n     //    gl_FrontLightProduct[0].specular * factor *\n     //    max( pow( dot( reflect, view ), gl_FrontMaterial.shininess ), 0.0 );\n     \n     // modified Blinn-Phong\n     vec3 halfway = normalize( light + view );\n     vec4 specular = \n         gl_FrontLightProduct[0].specular * factor *\n         max( pow( dot( normal, halfway ), gl_FrontMaterial.shininess ), 0.0 );\n     \n     // sum the components up, defaulting alpha to 1.0\n     gl_FragColor = \n         vec4( vec3( gl_FrontLightModelProduct.sceneColor + \n                     ambient + diffuse + specular ), 1.0 );\n }\n ";
