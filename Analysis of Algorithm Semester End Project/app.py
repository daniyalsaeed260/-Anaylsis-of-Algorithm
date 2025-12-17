import math
import random
import time
from flask import Flask, render_template, jsonify, request

app = Flask(__name__)

def distance(p1, p2):
    """Calculate Euclidean distance between two points."""
    return math.sqrt((p1[0] - p2[0])**2 + (p1[1] - p2[1])**2)

def brute_force_closest_pair(points):
    """
    Find the closest pair of points using brute force.
    O(n^2) complexity.
    """
    min_dist = float('inf')
    closest_pair = None
    n = len(points)
    
    for i in range(n):
        for j in range(i + 1, n):
            d = distance(points[i], points[j])
            if d < min_dist:
                min_dist = d
                closest_pair = (points[i], points[j])
                
    return closest_pair, min_dist

def strip_closest(strip, d):
    """
    Find the closest pair of points in the strip.
    Points in strip are sorted by Y coordinate.
    """
    min_dist = d
    closest_pair = None
    strip.sort(key=lambda point: point[1])
    
    for i in range(len(strip)):
        for j in range(i + 1, len(strip)):
            if (strip[j][1] - strip[i][1]) < min_dist:
                dist = distance(strip[i], strip[j])
                if dist < min_dist:
                    min_dist = dist
                    closest_pair = (strip[i], strip[j])
            else:
                break
    return closest_pair, min_dist

def closest_util(points_sorted_x):
    """
    Recursive function to find the smallest distance.
    """
    n = len(points_sorted_x)
    
    # Base case: Use brute force for small number of points
    if n <= 3:
        return brute_force_closest_pair(points_sorted_x)
    
    mid = n // 2
    mid_point = points_sorted_x[mid]
    
    left_points = points_sorted_x[:mid]
    right_points = points_sorted_x[mid:]
    
    pair_l, dist_l = closest_util(left_points)
    pair_r, dist_r = closest_util(right_points)
    
    if dist_l < dist_r:
        min_dist = dist_l
        closest_pair = pair_l
    else:
        min_dist = dist_r
        closest_pair = pair_r
        
    # Create strip
    strip = []
    for point in points_sorted_x:
        if abs(point[0] - mid_point[0]) < min_dist:
            strip.append(point)
            
    pair_strip, dist_strip = strip_closest(strip, min_dist)
    
    if dist_strip < min_dist:
        return pair_strip, dist_strip
    else:
        return closest_pair, min_dist

def divide_and_conquer_closest_pair(points):
    """
    Find the closest pair of points using Divide and Conquer.
    O(n log n) complexity.
    """
    points_sorted_x = sorted(points, key=lambda point: point[0])
    return closest_util(points_sorted_x)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/generate', methods=['POST'])
def generate_points():
    data = request.get_json()
    num_points = data.get('num_points', 50)
    # Generate points within a 800x600 canvas (with some padding)
    points = [(random.randint(20, 780), random.randint(20, 580)) for _ in range(num_points)]
    return jsonify(points)

@app.route('/closest', methods=['POST'])
def closest():
    data = request.get_json()
    points = data.get('points', [])
    
    if len(points) < 2:
        return jsonify({'error': 'Not enough points'}), 400
    
    # Brute Force
    start_time = time.time()
    bf_pair, bf_dist = brute_force_closest_pair(points)
    bf_time = (time.time() - start_time) * 1000 # ms
    
    # Divide and Conquer
    start_time = time.time()
    dnc_pair, dnc_dist = divide_and_conquer_closest_pair(points)
    dnc_time = (time.time() - start_time) * 1000 # ms
    
    return jsonify({
        'closest_pair_bruteforce': {
            'pair': bf_pair,
            'distance': bf_dist
        },
        'closest_pair_dnc': {
            'pair': dnc_pair,
            'distance': dnc_dist
        },
        'time_bruteforce': bf_time,
        'time_dnc': dnc_time
    })

if __name__ == '__main__':
    app.run(debug=True)
