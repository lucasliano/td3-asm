#include "filter.h"

// #pragma GCC target ("fpu=crypto-neon-fp-armv8")     // Adding compiler dependencies.. or smth like that..

// // Filter lenght = 16 always
// void fir_filter_neon(const int16_t *input, const int16_t *kernel, int16_t *output, int length) 
// {
//     // Ensure that length is a multiple of 8 (for 128-bit NEON registers)
//     int i;
//     for (i = 0; i < length - 7; i += 8) {
//         // Load input and kernel into NEON registers
//         int16x8_t input_vec = vld1q_s16(&input[i]);
//         int16x8_t kernel_vec = vld1q_s16(&kernel[i]);

//         // Perform the multiplication and accumulation
//         int32x4_t result_low = vmull_s16(vget_low_s16(input_vec), vget_low_s16(kernel_vec));
//         int32x4_t result_high = vmull_s16(vget_high_s16(input_vec), vget_high_s16(kernel_vec));

//         // Add the results to the output
//         int32x4_t output_vec = vld1q_s32(&output[i]);
//         output_vec = vaddq_s32(output_vec, result_low);
//         output_vec = vaddq_s32(output_vec, result_high);

//         // Store the result back to memory
//         vst1q_s32(&output[i], output_vec);
//     }

//     // Process any remaining elements (less than 8)
//     for (; i < length; ++i) {
//         for (int j = 0; j < 16; ++j) {
//             output[i] += input[i - j] * kernel[j];
//         }
//     }
// }

void fir_filter_neon(const float *input, const double *kernel, float *output, int length) 
{
    for (int i = 0; i < length; ++i) 
    {
        output[i] = 0;

        for (int j = 0; j < 12; ++j)    // coeff lenght = 12
        {
            if (i - j >= 0) {
                output[i] += input[i - j] * kernel[j];
            }
        }
    }
}