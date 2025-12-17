#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
};

double distance(const Point& p1, const Point& p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

pair<Point, Point> bruteForceClosestPair(const vector<Point>& points, double& minDist) {
    minDist = INFINITY;
    pair<Point, Point> closestPair;
    int n = points.size();
    
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double d = distance(points[i], points[j]);
            if (d < minDist) {
                minDist = d;
                closestPair = {points[i], points[j]};
            }
        }
    }
    return closestPair;
}

pair<Point, Point> stripClosest(vector<Point>& strip, double d, double& minDist) {
    minDist = d;
    pair<Point, Point> closestPair;
    
    sort(strip.begin(), strip.end(), [](const Point& a, const Point& b) {
        return a.y < b.y;
    });
    
    for (size_t i = 0; i < strip.size(); i++) {
        for (size_t j = i + 1; j < strip.size() && (strip[j].y - strip[i].y) < minDist; j++) {
            double dist = distance(strip[i], strip[j]);
            if (dist < minDist) {
                minDist = dist;
                closestPair = {strip[i], strip[j]};
            }
        }
    }
    return closestPair;
}

pair<Point, Point> closestUtil(vector<Point>& pointsSortedX, double& minDist) {
    int n = pointsSortedX.size();
    
    if (n <= 3) {
        return bruteForceClosestPair(pointsSortedX, minDist);
    }
    
    int mid = n / 2;
    Point midPoint = pointsSortedX[mid];
    
    vector<Point> left(pointsSortedX.begin(), pointsSortedX.begin() + mid);
    vector<Point> right(pointsSortedX.begin() + mid, pointsSortedX.end());
    
    double distL, distR;
    auto pairL = closestUtil(left, distL);
    auto pairR = closestUtil(right, distR);
    
    minDist = min(distL, distR);
    auto closestPair = (distL < distR) ? pairL : pairR;
    
    vector<Point> strip;
    for (const auto& point : pointsSortedX) {
        if (abs(point.x - midPoint.x) < minDist) {
            strip.push_back(point);
        }
    }
    
    double distStrip;
    auto pairStrip = stripClosest(strip, minDist, distStrip);
    
    if (distStrip < minDist) {
        minDist = distStrip;
        return pairStrip;
    }
    
    return closestPair;
}

pair<Point, Point> divideAndConquerClosestPair(vector<Point> points, double& minDist) {
    sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return a.x < b.x;
    });
    return closestUtil(points, minDist);
}

vector<Point> generateRandomPoints(int n) {
    vector<Point> points;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> disX(20, 780);
    uniform_real_distribution<> disY(20, 580);
    
    for (int i = 0; i < n; i++) {
        points.push_back(Point(disX(gen), disY(gen)));
    }
    return points;
}

void generateHTML(const vector<Point>& points, 
                  const pair<Point, Point>& bfPair, double bfDist, double bfTime,
                  const pair<Point, Point>& dncPair, double dncDist, double dncTime) {
    
    ofstream file("closest_pair_game.html");
    
    file << R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Closest Pair Quest - Results</title>
    <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600&display=swap" rel="stylesheet">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Outfit', sans-serif;
            background: linear-gradient(135deg, #0f172a 0%, #1e293b 100%);
            color: #f8fafc;
            min-height: 100vh;
            padding: 2rem;
        }
        .container { max-width: 1200px; margin: 0 auto; }
        header { text-align: center; margin-bottom: 2rem; }
        h1 {
            font-size: 3rem;
            background: linear-gradient(to right, #3b82f6, #8b5cf6);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            margin-bottom: 0.5rem;
        }
        .subtitle { color: #94a3b8; font-size: 1.2rem; }
        .canvas-container {
            background: #020617;
            border-radius: 1rem;
            padding: 1rem;
            margin-bottom: 2rem;
            box-shadow: 0 0 30px rgba(59, 130, 246, 0.2);
        }
        canvas {
            display: block;
            margin: 0 auto;
            background: #020617;
        }
        .stats {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 1rem;
            margin-bottom: 2rem;
        }
        .stat-card {
            background: #1e293b;
            padding: 1.5rem;
            border-radius: 0.75rem;
            border: 1px solid rgba(255,255,255,0.1);
        }
        .stat-card h3 {
            color: #94a3b8;
            font-size: 0.9rem;
            margin-bottom: 0.5rem;
            text-transform: uppercase;
        }
        .stat-card .value {
            font-size: 1.8rem;
            font-weight: 600;
            color: #3b82f6;
        }
        .winner {
            background: linear-gradient(135deg, #10b981 0%, #059669 100%);
            color: white;
            padding: 1rem;
            border-radius: 0.5rem;
            text-align: center;
            font-size: 1.2rem;
            font-weight: 600;
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>🎮 Closest Pair Quest</h1>
            <p class="subtitle">Algorithm Comparison Results</p>
        </header>
        
        <div class="canvas-container">
            <canvas id="canvas" width="800" height="600"></canvas>
        </div>
        
        <div class="stats">
            <div class="stat-card">
                <h3>Total Points</h3>
                <div class="value">)" << points.size() << R"(</div>
            </div>
            <div class="stat-card">
                <h3>Brute Force Time</h3>
                <div class="value">)" << fixed << setprecision(4) << bfTime << R"( ms</div>
            </div>
            <div class="stat-card">
                <h3>Divide & Conquer Time</h3>
                <div class="value">)" << fixed << setprecision(4) << dncTime << R"( ms</div>
            </div>
            <div class="stat-card">
                <h3>Closest Distance</h3>
                <div class="value">)" << fixed << setprecision(2) << dncDist << R"(</div>
            </div>
        </div>
        
        <div class="winner">
            ⚡ Divide & Conquer is )" << fixed << setprecision(2) << (bfTime / dncTime) << R"(x faster!
        </div>
    </div>
    
    <script>
        const canvas = document.getElementById('canvas');
        const ctx = canvas.getContext('2d');
        
        const points = )";
    
    // Output points as JSON
    file << "[";
    for (size_t i = 0; i < points.size(); i++) {
        file << "[" << points[i].x << "," << points[i].y << "]";
        if (i < points.size() - 1) file << ",";
    }
    file << "];\n";
    
    file << "        const closestPair = [[" << dncPair.first.x << "," << dncPair.first.y << "],["
         << dncPair.second.x << "," << dncPair.second.y << "]];\n";
    
    file << R"(
        // Draw all points
        ctx.fillStyle = '#3b82f6';
        points.forEach(p => {
            ctx.beginPath();
            ctx.arc(p[0], p[1], 4, 0, Math.PI * 2);
            ctx.fill();
        });
        
        // Draw line between closest pair
        ctx.strokeStyle = '#10b981';
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.moveTo(closestPair[0][0], closestPair[0][1]);
        ctx.lineTo(closestPair[1][0], closestPair[1][1]);
        ctx.stroke();
        
        // Highlight closest pair
        ctx.fillStyle = '#ef4444';
        closestPair.forEach(p => {
            ctx.beginPath();
            ctx.arc(p[0], p[1], 6, 0, Math.PI * 2);
            ctx.fill();
        });
    </script>
</body>
</html>)";
    
    file.close();
    cout << "\n✅ HTML file generated: closest_pair_game.html\n";
}

int main() {
    cout << "🎮 Closest Pair Quest - C++ Edition\n";
    cout << "====================================\n\n";
    
    int numPoints = 50;
    cout << "Generating " << numPoints << " random points...\n";
    vector<Point> points = generateRandomPoints(numPoints);
    
    // Brute Force
    cout << "Running Brute Force algorithm...\n";
    auto startBF = chrono::high_resolution_clock::now();
    double bfDist;
    auto bfPair = bruteForceClosestPair(points, bfDist);
    auto endBF = chrono::high_resolution_clock::now();
    double bfTime = chrono::duration<double, milli>(endBF - startBF).count();
    
    // Divide and Conquer
    cout << "Running Divide & Conquer algorithm...\n";
    auto startDNC = chrono::high_resolution_clock::now();
    double dncDist;
    auto dncPair = divideAndConquerClosestPair(points, dncDist);
    auto endDNC = chrono::high_resolution_clock::now();
    double dncTime = chrono::duration<double, milli>(endDNC - startDNC).count();
    
    // Display results
    cout << "\n📊 Results:\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << "Brute Force:\n";
    cout << "  Time: " << fixed << setprecision(4) << bfTime << " ms\n";
    cout << "  Distance: " << fixed << setprecision(2) << bfDist << "\n";
    cout << "  Pair: (" << bfPair.first.x << ", " << bfPair.first.y << ") - ("
         << bfPair.second.x << ", " << bfPair.second.y << ")\n\n";
    
    cout << "Divide & Conquer:\n";
    cout << "  Time: " << fixed << setprecision(4) << dncTime << " ms\n";
    cout << "  Distance: " << fixed << setprecision(2) << dncDist << "\n";
    cout << "  Pair: (" << dncPair.first.x << ", " << dncPair.first.y << ") - ("
         << dncPair.second.x << ", " << dncPair.second.y << ")\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    
    cout << "⚡ Speedup: " << fixed << setprecision(2) << (bfTime / dncTime) << "x faster with D&C!\n\n";
    
    // Generate HTML
    generateHTML(points, bfPair, bfDist, bfTime, dncPair, dncDist, dncTime);
    
    cout << "🌐 Open 'closest_pair_game.html' in your browser to see the visualization!\n";
    
    return 0;
}
