/*
 * aarch64_intrinsics.h - ARM64 intrinsic function mappings
 * Copyright (C) 2024 Amazon Q
 */

#ifndef __AARCH64_INTRINSICS_H
#define __AARCH64_INTRINSICS_H

/* Map function names to intrinsic opcodes for aarch64 */
typedef struct {
    const char *name;
    int opcode;
} intrinsic_mapping_t;

static const intrinsic_mapping_t aarch64_intrinsics[] = {
    {"COS", I_COS},
    {"SIN", I_SIN}, 
    {"TAN", I_TAN},
    {"ACOS", I_ACOS},
    {"ASIN", I_ASIN},
    {"ATAN", I_ATAN},
    {NULL, 0}
};

/* Function to check if a name is an intrinsic */
static inline int get_aarch64_intrinsic_opcode(const char *name) {
    const intrinsic_mapping_t *mapping = aarch64_intrinsics;
    while (mapping->name) {
        if (strcmp(mapping->name, name) == 0) {
            return mapping->opcode;
        }
        mapping++;
    }
    return 0;
}

#endif /* __AARCH64_INTRINSICS_H */