#include <stdlib.h>
#include <math.h>
#include <fftw3.h>

int main(void) {
    int i;
    double buffer[24];
    double* x = buffer;
    double* y = buffer+8;
    double* z = buffer+16;
    fftw_iodim dims[1];
    fftw_iodim howmany_dims[3];
    fftw_r2r_kind kind[1] = {FFTW_REDFT00};

    dims[0].n = 8;
    dims[0].is = 1;
    dims[0].os = 1;

    howmany_dims[0].n = 1;
    howmany_dims[0].is = 1;
    howmany_dims[0].os = 1;

    /* Uncomment one of these */
    fftw_plan p_dct = fftw_plan_r2r_1d(8, buffer, buffer+8, FFTW_REDFT10, FFTW_MEASURE);
    fftw_plan p_idct = 
        //fftw_plan_guru_r2r(1, dims, 1, howmany_dims, buffer, buffer, kind, FFTW_MEASURE);
        fftw_plan_r2r_1d(8, buffer+8, buffer+16, FFTW_REDFT01, FFTW_MEASURE);

    for (i = 0; i != 8; ++i)
    {
        x[i] = 0.0;
        y[i] = 0.0;
        z[i] = 0.0;
    }
    x[0] = 1.0;
    y[0] = 1.0;
    z[0] = 1.0;
    x[3] = 5;
    x[6] = 2;

    fftw_execute(p_dct);
    for (i = 0; i != 8; ++i)
        y[i] /= (2*8);

    fftw_execute(p_idct);
    for (i = 1; i != 8; ++i)
        z[i] /= (2*8);

    printf("Data:\nx: ");
    for (i = 0; i != 8; ++i) printf("%f, ", x[i]);
    printf("\ny: ");
    for (i = 0; i != 8; ++i) printf("%f, ", y[i]);
    printf("\nz: ");
    for (i = 0; i != 8; ++i) printf("%f, ", z[i]);
    printf("\n");

    return 0;
}

