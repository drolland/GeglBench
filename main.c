/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: drolland
 *
 * Created on 24 novembre 2016, 10:01
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib/gtypes.h>
#include "gegl-0.3/gegl.h"
#include "gegl-0.3/opencl/gegl-cl.h"


void gegl_operation_invalidate(GeglNode* node, GeglRectangle* r, gboolean b);

void g_object_print_all_properties(GObject* object) {
    guint size;
    GParamSpec** specs = g_object_class_list_properties(G_OBJECT_GET_CLASS(object), &size);
    for (int i = 0; i < size; i++) {
        GParamSpec* s = specs[i];
        GValue val = G_VALUE_INIT;
        g_printf("%s - %s \n", s->name, s->_blurb);
        g_param_spec_unref(s);
    }

}

#include <stdio.h>                                                                                                                                               
#include <stdlib.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#include <glib-2.0/glib/gdate.h>
#include <glib-2.0/glib/gtimer.h>
#include <glib-2.0/glib/gmem.h>
#endif

int list_devices() {

    int i, j;
    char* value;
    size_t valueSize;
    cl_uint platformCount;
    cl_platform_id* platforms;
    cl_uint deviceCount;
    cl_device_id* devices;
    cl_uint maxComputeUnits;

    // get all platforms
    clGetPlatformIDs(0, NULL, &platformCount);
    platforms = (cl_platform_id*) malloc(sizeof (cl_platform_id) * platformCount);
    clGetPlatformIDs(platformCount, platforms, NULL);

    for (i = 0; i < platformCount; i++) {

        // get all devices
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
        devices = (cl_device_id*) malloc(sizeof (cl_device_id) * deviceCount);
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);

        // for each device print critical attributes
        for (j = 0; j < deviceCount; j++) {

            // print device name
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, value, NULL);
            printf("%d. Device: %s\n", j + 1, value);
            free(value);

            // print hardware device version
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, valueSize, value, NULL);
            printf(" %d.%d Hardware version: %sn\n", j + 1, 1, value);
            free(value);

            // print software driver version
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
            printf(" %d.%d Software version: %sn\n", j + 1, 2, value);
            free(value);

            // print c version supported by compiler for device
            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
            value = (char*) malloc(valueSize);
            clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
            printf(" %d.%d OpenCL C version: %sn\n", j + 1, 3, value);
            free(value);

            // print parallel compute units
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS,
                    sizeof (maxComputeUnits), &maxComputeUnits, NULL);
            printf(" %d.%d Parallel compute units: %d\n", j + 1, 4, maxComputeUnits);

        }

        free(devices);

    }

    free(platforms);
    return 0;

}

int main(int argc, char** argv) {

    gegl_init(&argc, &argv);
    if ( gegl_cl_is_accelerated() )
        g_message("OpenCL acceleration available in gegl");

    GeglNode *gegl = gegl_node_new();

    GeglNode *file = gegl_node_new_child(gegl, "operation", "gegl:load", "path", "image2.jpg", NULL);

    GeglNode *blur = gegl_node_new_child(gegl, "operation", "gegl:box-blur", "radius", 50, NULL);


    g_object_print_all_properties(G_OBJECT(blur));

    GValue val_object = G_VALUE_INIT;
    g_value_init(&val_object, G_TYPE_OBJECT);
    g_object_get_property(G_OBJECT(blur), "gegl-operation", &val_object);
    GeglOperation* blur_op = GEGL_OPERATION(g_value_get_object(&val_object));

    g_object_print_all_properties(G_OBJECT(blur_op));

    GValue val = G_VALUE_INIT;
    g_value_init(&val, G_TYPE_BOOLEAN);

    

    gegl_node_link_many(file, blur, NULL);

    list_devices();

    // print device name

    char* value;
    int valueSize;

    cl_device_id device = gegl_cl_get_device();

    clGetDeviceInfo(device, CL_DEVICE_NAME, 0, NULL, &valueSize);
    value = (char*) malloc(valueSize);
    clGetDeviceInfo(device, CL_DEVICE_NAME, valueSize, value, NULL);
    printf("Device : %s", value);
    free(value);

    /*
                // print hardware device version
                clGetDeviceInfo(device, CL_DEVICE_VERSION, 0, NULL, &valueSize);
                value = (char*) malloc(valueSize);
                clGetDeviceInfo(device, CL_DEVICE_VERSION, valueSize, value, NULL);
                printf(" %d.%d Hardware version: %sn", j+1, 1, value);
                free(value);
 
                // print software driver version
                clGetDeviceInfo(device, CL_DRIVER_VERSION, 0, NULL, &valueSize);
                value = (char*) malloc(valueSize);
                clGetDeviceInfo(device, CL_DRIVER_VERSION, valueSize, value, NULL);
                printf(" %d.%d Software version: %sn", j+1, 2, value);
                free(value);
 
                // print c version supported by compiler for device
                clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
                value = (char*) malloc(valueSize);
                clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
                printf(" %d.%d OpenCL C version: %sn", j+1, 3, value);
                free(value);
 
                // print parallel compute units
                clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS,
                        sizeof(maxComputeUnits), &maxComputeUnits, NULL);
                printf(" %d.%d Parallel compute units: %dn", j+1, 4, maxComputeUnits);
 
            }
     */


    GTimer* timer = g_timer_new();

    gboolean b = 0;
    g_value_set_boolean(&val, b);
    g_object_set_property(G_OBJECT(blur), "use-opencl", &val);

    int i = 0;
    while (i < 10) {
        /* Dirty hack to force node invalidation. Necessary to run the process function multiple times*/
        gegl_node_set_passthrough(blur, TRUE);
        gegl_node_set_passthrough(blur, FALSE);
        
        gegl_node_process(blur);
        i++;
    }

    double elapsed = g_timer_elapsed(timer,NULL);
    g_timer_start(timer);
    
    g_message("No OpenCL Time elapsed %f",elapsed);
    
    g_timer_start(timer);

    b = 1;
    g_value_set_boolean(&val, b);
    g_object_set_property(G_OBJECT(blur), "use-opencl", &val);

    i = 0;
    while (i < 10) {
        /* Dirty hack to force node invalidation. Necessary to run the process function multiple times*/
        gegl_node_set_passthrough(blur, TRUE);
        gegl_node_set_passthrough(blur, FALSE);
        
        gegl_node_process(blur);
        i++;
    }

    elapsed = g_timer_elapsed(timer,NULL);
    g_timer_start(timer);
    
    g_message("With OpenCL Time elapsed %f",elapsed);
    
    g_free(timer);
    return (EXIT_SUCCESS);
}

