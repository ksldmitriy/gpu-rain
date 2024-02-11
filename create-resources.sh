#!/bin/bash

SHADERS_DIR=$(dirname "$0")/shaders
RESOURCE_DIR=$(dirname "$0")/resources
RESOURCE_FILE=$RESOURCE_DIR/shader-resources.s
RESOURCE_HEADER=$RESOURCE_DIR/shader-resources.hpp

mkdir -p $RESOURCE_DIR

SHADER_RESOURCE_TEMPLATE="
    .global tmpname_shader_resource
    .type   tmpname_shader_resource, @object
    .balign 4
tmpname_shader_resource:
    .incbin \"tmpfile\"
tmpname_shader_resource_end:
    .global tmpname_shader_resource_size,
    .type   tmpname_shader_resource_size, @object
    .balign 4
tmpname_shader_resource_size:
    .int tmpname_shader_resource_end - tmpname_shader_resource"

SHADER_RESOURCE_HEADER_TEMPLATE="
	extern const char tmpname_shader_resource[];
	extern const int tmpname_shader_resource_size;"

echo "    .section .rodata" > $RESOURCE_FILE

echo '#pragma once

extern "C" {' > $RESOURCE_HEADER

for file in $(find "$SHADERS_DIR" -type f \( -name "*.vert" -o -name "*.frag" \)); do
	file_name=$(basename "$file")
	sed "s/tmpname/${file_name//./_}/g; s/tmpfile/$file_name/g" <<< $SHADER_RESOURCE_TEMPLATE >> $RESOURCE_FILE
	sed "s/tmpname/${file_name//./_}/g" <<< $SHADER_RESOURCE_HEADER_TEMPLATE >> $RESOURCE_HEADER
done

echo '
}' >> $RESOURCE_HEADER
