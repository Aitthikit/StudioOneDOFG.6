/*
 * kalman_filter.c
 *
 *  Created on: May 20, 2024
 *      Author: Qzider
 */
// kalman_filter.c
#include "kalman_filter.h"

// Define constants
float Ak[3][3] = {
    {1, DT, 0.5f * DT * DT},
    {0, 1, DT},
    {0, 0, 1}
};
float B[3][1] = {0, 0, 0};
float Ck[1][3] = {1, 0, 0};
float G[3][1] = {0.5f * DT * DT * 0.0447f, DT * 0.0447f, 0.0447f};
//float G[3][1] = {0,0,0};
float R = 3.5f;
float Q = 1.0f;

// Initial state and covariance
float x_est[3] = {0, 0, 0};
float P_est[3][3] = {0};

float y;
// Matrix operations
void mat_mult(float a[3][3], float b[3][3], float result[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void mat_add(float a[3][3], float b[3][3], float result[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[i][j] = a[i][j] + b[i][j];
        }
    }
}

void mat_sub(float a[3][3], float b[3][3], float result[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[i][j] = a[i][j] - b[i][j];
        }
    }
}

void mat_transpose(float a[1][3], float result[3][1]) {
    for (int i = 0; i < 3; i++) {
        result[i][0] = a[0][i];
    }
}

void mat_scalar_mult(float a[3][3], float scalar, float result[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[i][j] = a[i][j] * scalar;
        }
    }
}

void mat_transpose_3x3(float a[3][3], float result[3][3]) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			result[j][i] = a[i][j];
		}
	}
}
void mat_transpose_3x1(float a[3][1], float result[1][3]) {
    for (int i = 0; i < 3; i++) {
        result[0][i] = a[i][0];
    }
}

void mat_mult_1x3_3x3(float a[1][3], float b[3][3], float result[1][3]) {
    for (int i = 0; i < 3; i++) {
        result[0][i] = 0;
        for (int j = 0; j < 3; j++) {
            result[0][i] += a[0][j] * b[j][i];
        }
    }
}
void mat_mult_3x1_1x3(float a[3][1], float b[1][3], float result[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[i][j] = a[i][0] * b[0][j]; // Perform the multiplication for each element
        }
    }
}
void mat_mult_1x3_3x1(float a[1][3], float b[3][1], float *result) {
    *result = 0; // Initialize the result as 0
    for (int i = 0; i < 3; i++) {
        *result += a[0][i] * b[i][0]; // Perform the multiplication and accumulate
    }
}

void mat_mult_3x3_3x1(float a[3][3], float b[3][1], float result[3][1]) {
    for (int i = 0; i < 3; i++) {
        result[i][0] = 0; // Initialize each element of the result
        for (int j = 0; j < 3; j++) {
            result[i][0] += a[i][j] * b[j][0]; // Perform multiplication and accumulate
        }
    }
}
void mat_mult_3x3_1x3(float a[3][3], float b[1][3], float result[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[i][j] = a[i][j] * b[0][j]; // Perform the multiplication for each element
        }
    }
}
void kalman_predict(float x_est[3], float P_est[3][3], float A[3][3], float G[3][1], float Q) {
    // x_pred = A * x_est
    float x_pred[3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            x_pred[i] += Ak[i][j] * x_est[j];
        }
    }

    // P_pred = A * P_est * A' + G * Q * G'
    float A_T[3][3];
    mat_transpose_3x3(Ak, A_T);

    float temp1[3][3];
    mat_mult(Ak, P_est, temp1);

    float temp2[3][3];
    mat_mult(temp1, A_T, temp2);

    float G_Q[3][1];
    for (int i = 0; i < 3; i++) {
		G_Q[i][0] = G[i][0] * Q;
	}
//    mat_scalar_mult(G, Q, G_Q);

    float G_T[1][3];
    mat_transpose_3x1(G, G_T);

    float G_Q_G_T[3][3];
    mat_mult_3x1_1x3(G_Q, G_T, G_Q_G_T);

    mat_add(temp2, G_Q_G_T, P_est);

    // Update the state estimate
    for (int i = 0; i < 3; i++) {
        x_est[i] = x_pred[i];
    }
}

void kalman_update(float x_est[3], float P_est[3][3], float C[1][3], float R, float z) {
    // y = z - C * x_pred
    float C_x[1] = {0};
    for (int i = 0; i < 3; i++) {
        C_x[0] += Ck[0][i] * x_est[i];
    }

    y = z - C_x[0];

    // S = C * P_pred * C' + R
    float C_T[3][1];
    mat_transpose(Ck, C_T);

    float temp1[1][3];
    mat_mult_1x3_3x3(Ck, P_est, temp1);

    float temp2;
    mat_mult_1x3_3x1(temp1, C_T, &temp2);

    float S = temp2 + R;

    // K = P_pred * C' / S
    float temp3[3][1];
    mat_mult_3x3_3x1(P_est, C_T, temp3);

    float K[3][1];
    for (int i = 0; i < 3; i++) {
        K[i][0] = temp3[i][0] / S;
    }

    // x_est = x_pred + K * y
    for (int i = 0; i < 3; i++) {
        x_est[i] += K[i][0] * y;
    }

    // P_est = (I - K * C) * P_pred
    float K_C[3][3];
    mat_mult_3x3_1x3(K, Ck, K_C);

    float I[3][3] = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };

    float I_minus_K_C[3][3];
    mat_sub(I, K_C, I_minus_K_C);

    float new_P[3][3];
    mat_mult(I_minus_K_C, P_est, new_P);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            P_est[i][j] = new_P[i][j];
        }
    }
}


