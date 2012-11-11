#define GLEW_STATIC
#include "include/GL/glew.h"
#define FREEGLUT_STATIC
#include "include/GL/glut.h"
#include <stdio.h>
#include <math.h>
#include "include/vector.h"
#include "include/matrix.h"
#include "include/shader.h"
#include "include/geom_format.h"
#include "include/file_manager.h"
#include "include/jpeglib.h"
#include <stdlib.h>

ShaderProg shaderProg;
GLuint program;
GLint viewport_width_ = 500;
GLint viewport_height_ = 500;
int horizontal_steps_ = 8;  //4 + multiple of 4
int vertical_steps_ = 5;    //3 + multiple of 2
mat4 modelToProjection_;
mat4 cameraToWorld;
mat4 modelToWorld;
char default_source[] = "assets/duck.geom";
char * source;

void updateView()
{
  // flip it around to transform from world to camera
  mat4 worldToCamera;
  cameraToWorld.invertQuick(worldToCamera);

  // build a projections matrix to add perspective
  mat4 cameraToProjection;
  cameraToProjection.loadIdentity();
  float n = 0.1f, f = 200;
  cameraToProjection.frustum(-n, n, -n, n, n, f);

  // model -> world -> camera -> projection
  modelToProjection_ = modelToWorld * worldToCamera * cameraToProjection;
}

void screendump() {

  int bytes_per_pixel = 3;   // or 1 for GRACYSCALE images
  GLubyte *raw_image = new GLubyte[viewport_width_*viewport_height_*3];

  // make buffer
  JSAMPROW row_pointer[1];
	// read the jpeg into the raw buffer
	row_pointer[0] = (GLubyte *)malloc( viewport_width_ * 3 );
  glReadPixels(0, 0, viewport_width_, viewport_height_, GL_RGB, GL_UNSIGNED_BYTE, raw_image);

  char *filename = "impostors.jpg";

  struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	
	// this is a pointer to one row of image data
	FILE *outfile = fopen( filename, "wb" );

	if ( !outfile )
	{
		printf("Error opening output jpeg file %s\n!", filename );
    return;
	}
	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);

	// Setting the parameters of the output file here
	cinfo.image_width = viewport_width_;	
	cinfo.image_height = viewport_height_;
	cinfo.input_components = bytes_per_pixel;
	cinfo.in_color_space = JCS_RGB;
  // default compression parameters, we shouldn't be worried about these
	jpeg_set_defaults( &cinfo );
	// Now do the compression ..
	jpeg_start_compress( &cinfo, TRUE );

	// write one row at a time
	while( cinfo.next_scanline < cinfo.image_height )
	{
		row_pointer[0] = &raw_image[ (cinfo.image_height - cinfo.next_scanline) * cinfo.image_width *  cinfo.input_components];
// THIS FAILS:
// it used to fail .. then I compiled my own libjpeg.lib .. then it worked .. haven't changed anything to the project .. and now it fails again
		jpeg_write_scanlines( &cinfo, row_pointer, 1 );
	}

	// similar to read file, clean up after we're done compressing
	jpeg_finish_compress( &cinfo );
	jpeg_destroy_compress( &cinfo );
	fclose( outfile );
}

void display()
{
  vec4 color(1, 1, 0, 1);
  vec4 light_direction(-1, 1, 1, 1);
  shaderProg.setSomeStuff(color, light_direction);
  shaderProg.setMatrixStuff(modelToProjection_);
  
  glClearColor(0, 0, 0, 1);
  glViewport(0, 0, viewport_width_, viewport_height_);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  const unsigned char *src = (const unsigned char *)file_manager::bytes(source);
  const geom_format::header_t &hdr = (geom_format::header_t &)src[0];
   
  for (int i = 0; i != hdr.num_elements; ++i) {
    // for each element
    const geom_format::element_t &element = (const geom_format::element_t &)src[hdr.header_size + i * hdr.element_size];
    for (int j = 0; j != element.num_attrs; ++j) {
      // for each attribute of element
      const geom_format::attr_t &attr = element.attrs[j];
      glVertexAttribPointer(attr.index, attr.size, GL_BYTE + attr.type, GL_FALSE, element.stride, (void*)( src + element.vertices + attr.offset ));
      glEnableVertexAttribArray(attr.index);
    }

    for (int k = 0; k < vertical_steps_; k++) {
      for (int l = 0; l < horizontal_steps_; l++) {
        glViewport(viewport_width_ / horizontal_steps_ * l, viewport_height_ / vertical_steps_ * k, 100, 100);

        updateView();

        modelToProjection_.rotateX(180 / (vertical_steps_ -1) * k - 90);
        modelToProjection_.rotateY(360 / horizontal_steps_ * l);
        
        shaderProg.setMatrixStuff(modelToProjection_);

        glDrawArrays(GL_TRIANGLES, 0, element.num_vertices); 
      }
    }
  }

  screendump();
}

// boiler plate for building a camera
void build_camera_matrix()
{
  // this matrix can be used to move objects around in the scene
  modelToWorld.loadIdentity();

  // this is the transform from the camera to the world
  cameraToWorld.loadIdentity();
  cameraToWorld.translate(0, 0, 5);
}

void main(int argc, char** argv) {

  if(argc < 2) {
    // print usage message, quit
    printf(
      "\r\n"
      "Usage: impostorise.exe [source] [horizontal steps] [vertical steps] \r\n"
      "- [source] must be a .geom file \r\n"
      "- [horizontal steps] number of angles horizontally (4 + multiple of 4) \r\n"
      "- [vertical steps] number of angles vertically (3 + multiple of 2) \r\n"
      "\r\n"
      "Defaults used - [assets/duck.geom] [8] [5] \r\n"
      "\r\n"
      "Outputs to impostors.jpg"
      "\r\n"
    );
    system ("pause");
  }

  // not doing any checks .. just assuming it's all in order
  if(argc > 1 ) {
    source = argv[1];
  } else {
    source = default_source;
  }
  if(argc > 3 ) {
    horizontal_steps_ = atoi(argv[2]);
    vertical_steps_ = atoi(argv[3]);
  }

  viewport_width_ = 100 * horizontal_steps_;
  viewport_height_ = 100 * vertical_steps_;

  glutInit(&argc, argv);
  glutInitWindowSize(viewport_width_, viewport_height_);
  glutCreateWindow("Impostorise!");
  glewInit();

  shaderProg.init();
  program = shaderProg.program();

  build_camera_matrix();

  //function for rendering
  glutDisplayFunc(display);

  glutMainLoop(); //Start the main loop
}