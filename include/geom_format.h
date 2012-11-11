////////////////////////////////////////////////////////////////////////
//
// format of the ".geom" file
//
// (C) Andy Thomaosn 2011
//
namespace geom_format {
  typedef unsigned char u8;
  typedef unsigned short u16;
  typedef unsigned int u32;

  // output attribute, corresponds to glVertexAttribPointer
  struct attr_t {
    u8 index;
    u8 size;
    u8 type; // GL_BYTE = 0x14(00) .. GL_FLOAT = 0x14(06)
    u8 offset;
  };

  // element, corresponds to a call to glDrawArrays/glDrawElements
  struct element_t {
    enum { max_attrs = 8 };
    u32 indices;      // offset from start of file to indices
    u32 vertices;     // offset from start of file to vertices
    u32 num_indices;
    u32 num_vertices;
    u8 num_attrs;
    u8 stride;
    u16 material;
    attr_t attrs[max_attrs];
  };

  // header for the file
  struct header_t {
    char sig[4];
    u16 header_size;
    u16 num_elements;
    u16 element_size;
    u16 attr_size;
  };
}
