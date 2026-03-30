#include <cif.h>
#include <stdarg.h>

void ProcPar (Workspace wptr, int numprocs, ...)
{
        LightProcBarrier bar;
        va_list ap;

        LightProcBarrierInit (wptr, &bar, numprocs);

        va_start (ap, numprocs);
        while (numprocs-- > 0) {
                Workspace ws = va_arg (ap, Workspace);
                Process func = va_arg (ap, Process);

                LightProcStart (wptr, &bar, ws, func);
        }
        va_end (ap);

        LightProcBarrierWait (wptr, &bar);
}

int ProcAlt (Workspace wptr, ...)
{
        int i, fired = -1;
        va_list ap;

        Alt (wptr);

        /*{{{ enable */
        va_start (ap, wptr);
        for (i = 0; ; i++) {
                Channel *c = va_arg (ap, Channel *);
                if (c == NULL)
                        break;

                if (AltEnableChannel (wptr, i, c)) {
                        fired = i;
                        break;
                }
        }
        va_end (ap);
        /*}}}*/

        if (fired == -1) {
                AltWait (wptr);
        }

        /*{{{ disable */
        va_start (ap, wptr);
        for (i = 0; ; i++) {
                Channel *c = va_arg (ap, Channel *);
                if (c == NULL)
                        break;

                AltDisableChannel (wptr, i, c);

                if (i == fired)
                        break;
        }
        va_end (ap);
        /*}}}*/

        return AltEnd (wptr);
}

mt_array_t *MTAllocArray (Workspace wptr, word element_type, int dimensions, ...)
{
        va_list ap;
        int size = 1;
        mt_array_t *array;
        int i;

        va_start (ap, dimensions);
        for (i = 0; i < dimensions; i++)
                size *= va_arg (ap, int);
        va_end (ap);

        array = MTAlloc (wptr, MT_MAKE_ARRAY_TYPE (dimensions, element_type), size);

        va_start (ap, dimensions);
        for (i = 0; i < dimensions; i++)
                array->dimensions[i] = va_arg (ap, int);
        va_end (ap);

        return array;
}

mt_array_t *MTAllocDataArray (Workspace wptr, int element_size, int dimensions, ...)
{
        va_list ap;
        int size = element_size;
        mt_array_t *array;
        int i;

        va_start (ap, dimensions);
        for (i = 0; i < dimensions; i++)
                size *= va_arg (ap, int);
        va_end (ap);

        array = MTAlloc (wptr, MT_MAKE_ARRAY_TYPE (dimensions, (MT_MAKE_NUM(MT_NUM_BYTE))), size);

        va_start (ap, dimensions);
        for (i = 0; i < dimensions; i++)
                array->dimensions[i] = va_arg (ap, int);
        va_end (ap);

        return array;
}