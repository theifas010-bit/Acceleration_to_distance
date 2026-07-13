// =====================================================
// ELEC1006 - Task 1
// Acceleration to Distance Calculation
// =====================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>

using namespace std;

int main() {

    vector<double> time;
    vector<double> ax;
    vector<double> ay;
    vector<double> az;

    ifstream file("task1_acceleration_2.csv");

    if (!file.is_open()) {
        cout << "Cannot open file\n";
        return 1;
    }

    string line;
    string value;

    // Skip header line
    getline(file, line);

    // Read CSV file
    while (getline(file, line)) {

        stringstream ss(line);
        string value;

        double t, x, y, z;

        getline(ss, value, ',');
        if (value.empty()) continue;
        t = stod(value);

        getline(ss, value, ',');
        if (value.empty()) continue;
        x = stod(value);

        getline(ss, value, ',');
        if (value.empty()) continue;
        y = stod(value);

        getline(ss, value, ',');
        if (value.empty()) continue;
        z = stod(value);

        time.push_back(t);
        ax.push_back(x);
        ay.push_back(y);
        az.push_back(z);
    }
    file.close();

    int n = time.size();

    if (n < 2) {
        cout << "Not enough data\n";
        return 1;
    }

    // -------------------------------------------------
    // Remove average sensor offset
    // This reduces drift caused by small sensor bias
    // -------------------------------------------------
    double meanX = 0;
    double meanY = 0;
    double meanZ = 0;

    for (int i = 0; i < n; i++) {
        meanX += ax[i];
        meanY += ay[i];
        meanZ += az[i];
    }

    meanX = meanX / n;
    meanY = meanY / n;
    meanZ = meanZ / n;

    for (int i = 0; i < n; i++) {
        ax[i] = ax[i] - meanX;
        ay[i] = ay[i] - meanY;
        az[i] = az[i] - meanZ;
    }

    // Velocity vectors
    vector<double> vx(n, 0);
    vector<double> vy(n, 0);
    vector<double> vz(n, 0);

    // Displacement in each direction
    vector<double> dx(n, 0);
    vector<double> dy(n, 0);
    vector<double> dz(n, 0);

    // Total cumulative distance travelled
    vector<double> distance(n, 0);

    // -------------------------------------------------
    // Trapezoidal integration
    // Acceleration -> Velocity -> Distance
    // -------------------------------------------------
    for (int i = 1; i < n; i++) {

        double dt = time[i] - time[i - 1];

        // Acceleration to velocity
        vx[i] = vx[i - 1] + 0.5 * (ax[i - 1] + ax[i]) * dt;
        vy[i] = vy[i - 1] + 0.5 * (ay[i - 1] + ay[i]) * dt;
        vz[i] = vz[i - 1] + 0.5 * (az[i - 1] + az[i]) * dt;

        // Velocity to displacement for this small interval
        double sx = 0.5 * (vx[i - 1] + vx[i]) * dt;
        double sy = 0.5 * (vy[i - 1] + vy[i]) * dt;
        double sz = 0.5 * (vz[i - 1] + vz[i]) * dt;

        // Cumulative displacement in each direction
        dx[i] = dx[i - 1] + sx;
        dy[i] = dy[i - 1] + sy;
        dz[i] = dz[i - 1] + sz;

        // Segment distance travelled
        double segmentDistance = sqrt(sx*sx + sy*sy + sz*sz);

        // Cumulative total distance
        distance[i] = distance[i - 1] + segmentDistance;
    }

    // Print some results
    cout << "Time\tDX\tDY\tDZ\tTotal\n";

    for (int i = 0; i < n; i += 10) {
        cout << fixed << setprecision(4)
             << setw(10) << time[i]
             << setw(15) << dx[i]
             << setw(15) << dy[i]
             << setw(15) << dz[i]
             << setw(15) << distance[i] << endl;
    }

    cout << "\nFinal estimated distance = "
         << distance[n - 1] << " m\n";

    // Save results to CSV
    ofstream out("results.csv");

    out << "Time,DX,DY,DZ,Total_Distance\n";

    for (int i = 0; i < n; i++) {
        out << time[i] << ","
            << dx[i] << ","
            << dy[i] << ","
            << dz[i] << ","
            << distance[i] << "\n";
    }

    out.close();

    cout << "Results saved to results.csv\n";

    return 0;
}