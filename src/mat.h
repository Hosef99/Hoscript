#ifndef MAT_H
#define MAT_H

typedef enum
{
    VAL_NUM,
    VAL_MAT,
    VAL_EMP
} ValueType;

typedef struct
{
    ValueType type;
    union
    {
        double number;
        struct
        {
            int rows;
            int cols;
            double** data;
        }mat;
    };
    
} Value;

#endif