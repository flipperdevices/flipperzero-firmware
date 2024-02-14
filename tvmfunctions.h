// Schaene Trible
// This File contains functions for solving TVM problems
// Everything Here assumes each number is filled out, whether a 0 or a value
// Also assumes end of period interest

#include <math.h>

// Returns Future value, regular TVM calculation
double findFV(double tvm_n, double tvm_i, double tvm_pv, double tvm_pmt) {
    return (tvm_pv * pow(1 + tvm_i, tvm_n) + tvm_pmt * ((pow(1 + tvm_i, tvm_n) - 1) / tvm_i)) * -1;
}

// Returns Present value, regular TVM calculation
double findPV(double tvm_n, double tvm_i, double tvm_pmt, double tvm_fv) {
    return -tvm_fv / pow(1 + tvm_i, tvm_n) - (tvm_pmt / tvm_i) * (1 - pow(1 + tvm_i, -tvm_n));
}

// helper function for findI, assumes the attemped i is correct and returns a number to compare.
// function hopes to return basically 0
double
    evaluateTVMI(double attempted_i, double tvm_n, double tvm_pmt, double tvm_pv, double tvm_fv) {
    // If 0, skip all the extra processing
    if(attempted_i == 0) {
        return tvm_pv + tvm_pmt * tvm_n + tvm_fv;
    } else {
        return tvm_pv * pow(attempted_i + 1, tvm_n) +
               tvm_pmt * (pow(attempted_i + 1, tvm_n) - 1) / attempted_i + tvm_fv;
    }
}

// Returns I, by narrowing down on it.
double findI(double tvm_n, double tvm_pmt, double tvm_pv, double tvm_fv) {
    double epsilon = 0.0000001;
    double step = 0.00001;
    // Give up after 128 times.
    double maxIterations = 128;

    // If N is 0, Interest doesn't quite have a purpose
    if(tvm_n <= 0) {
        return NAN;
    }

    // Make a first Simple guess, 10%
    double Rate0 = .1;
    double Y0 = evaluateTVMI(Rate0, tvm_n, tvm_pmt, tvm_pv, tvm_fv);
    // If that attempt is > 0, was too high. guess 5% next. if <= 0, too low. guess 20%
    double Rate1 = Y0 > 0 ? Rate0 / 2 : Rate0 * 2;
    // Guess again with the new rate.
    double Y1 = evaluateTVMI(Rate1, tvm_n, tvm_pmt, tvm_pv, tvm_fv);

    // Iteration time
    int i = 0;
    while(i < maxIterations) {
        if(Y1 == Y0) {
            // If the same, step rate0 down if smaller, up if bigger
            Rate0 = Rate0 < Rate1 ? Rate0 - step : Rate0 + step;
            // eval Y0 again
            Y0 = evaluateTVMI(Rate0, tvm_n, tvm_pmt, tvm_pv, tvm_fv);
        }
        // If they're still the same, somethings wrong
        if(Y1 == Y0) {
            return NAN;
        }

        Rate0 = Rate1 - (Rate1 - Rate0) * Y1 / (Y1 - Y0);
        Y0 = evaluateTVMI(Rate0, tvm_n, tvm_pmt, tvm_pv, tvm_fv);

        // If the absolute value of Y0 is basically 0, we found the rate
        if(fabs(Y0) < epsilon) {
            return Rate0;
        } else {
            // Swap out for the next iteration
            double tempVar = Y0;
            Y0 = Y1;
            Y1 = tempVar;
            tempVar = Rate0;
            Rate0 = Rate1;
            Rate1 = tempVar;
        }
        i++;
    }
    // Struck out
    return NAN;
}

double FindN(double tvm_i, double tvm_pv, double tvm_pmt, double tvm_fv) {
    if(tvm_i == 0) {
        if(tvm_pmt == 0) {
            return NAN;
        } else {
            // if no interest or pmt, just do the basic equation
            return -(tvm_pv + tvm_fv) / tvm_pmt;
        }
    }
    // If there is an interest rate
    else {
        double tempVarFV = -tvm_fv + (tvm_pmt / tvm_i);
        double tempVarPV = tvm_pv + (tvm_pmt / tvm_i);

        // Test to ensure values fit within log() function
        if(tempVarFV < 0 && tempVarPV < 0) {
            // if both netagive, make positive
            tempVarFV = -tempVarFV;
            tempVarPV = -tempVarPV;
        }
        // if both negative or 0, error
        else if(tempVarFV <= 0 || tempVarPV <= 0) {
            return NAN;
        }

        return (log(tempVarFV) - log(tempVarPV)) / log(tvm_i + 1);
    }
}