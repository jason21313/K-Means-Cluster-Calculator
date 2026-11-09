import matplotlib.pyplot as plt
import subprocess, sys
from dataclasses import dataclass, field

# Fixed colour order: cluster 1 is always blue, cluster 2 always orange, etc.
CLUSTER_COLORS = ["#2a78d6", "#eb6834", "#1baf7a", "#eda100",
                  "#e87ba4", "#008300", "#7a5cd6", "#c23b3b"]
# A different marker shape per cluster, so clusters are still distinguishable
# for colour-blind readers or in black-and-white print.
CLUSTER_MARKERS = ["o", "s", "^", "D", "v", "P", "X", "*"]


@dataclass
class Data:
    dim: int
    var: float
    k: int
    centroids: list                              # k tuples, each of length dim
    points: list                                 # tuples, each of length dim
    labels: list = field(default_factory=list)   # point names (may be empty)

def process_file(filename: str) -> Data:
    """
    File format (written by the C program):
        line 1:      dim variance k has_names
        next:        the k centroids (dim numbers each)
        remaining:   one point per line, optionally prefixed by its name
    """
    with open(filename) as file:
        lines = [line.strip() for line in file if line.strip()]

    header = lines[0].split()
    dim, var, k, has_names = int(header[0]), float(header[1]), int(header[2]), header[3] == "1"

    numbers = []
    centroids = []
    line_no = 1
    while len(numbers) < k * dim:
        numbers += [float(x) for x in lines[line_no].split()]
        line_no += 1
    for c in range(k):
        start = c * dim
        end = (c+1)*dim
        centroids.append(tuple(numbers[start:end]))

    points, labels = [], []
    for line in lines[line_no:]:
        parts = line.split()
        if has_names:
            labels.append(parts[0])
            parts = parts[1:]
        points.append(tuple(float(x) for x in parts[:dim]))

    return Data(dim, var, k, centroids, points, labels)

def assign_clusters(data: Data)->list:
    assignments = []
    for p in data.points:
        dists = []
        for c in data.centroids:
            dists.append(sum((p[d]-c[d])**2 for d in range(data.dim))**.5)
        assignments.append(dists.index(min(dists)))
    return assignments

def plot_data(data: Data):
    assignments = assign_clusters(data)

    if data.dim == 3:
        fig = plt.figure(figsize=(7, 6))
        ax = fig.add_subplot(projection="3d")
    else:
        fig, ax = plt.subplots(figsize=(7, 5 if data.dim == 2 else 2.5))

    def coords(p):
        # 1-D data sits on a horizontal line at y = 0
        return (p[0], 0) if data.dim == 1 else p

    for c in range(data.k):
        color = CLUSTER_COLORS[c % len(CLUSTER_COLORS)]
        marker = CLUSTER_MARKERS[c % len(CLUSTER_MARKERS)]

        # All points in cluster c, drawn in one call so they share a legend entry
        members = [coords(p) for p, a in zip(data.points, assignments) if a == c]
        if members:
            ax.scatter(*zip(*members), color=color, marker=marker, s=60,
                       edgecolors="white", linewidths=1, label=f"Cluster {c + 1}",
                       zorder=3)

        # The centroid: same colour, big cross, drawn underneath the points
        ax.scatter(*coords(data.centroids[c]), color=color, marker="X", s=220,
                   edgecolors="black", linewidths=1, alpha=0.6, zorder=2)

    # Name each point (fine for small data sets; remove for large ones)
    for p, name in zip(data.points, data.labels):
        if data.dim == 3:
            ax.text(*p, f"  {name}", fontsize=9, color="#444")
        else:
            ax.annotate(name, coords(p), xytext=(6, 6), textcoords="offset points",
                        fontsize=9, color="#444")

    # Legend entry explaining the centroid marker
    ax.scatter([], [], color="gray", marker="X", s=120, edgecolors="black", label="Centroid")

    ax.set_title(f"K-means clusters (k={data.k}, variance={data.var:g})")
    ax.set_xlabel("x")
    if data.dim == 1:
        ax.set_yticks([])
    else:
        ax.set_ylabel("y")
    if data.dim == 3:
        ax.set_zlabel("z")
    ax.legend(frameon=False, loc="upper left", bbox_to_anchor=(1.02, 1))
    ax.grid(alpha=0.3)
    plt.tight_layout()
    plt.show()

def main():
    build = subprocess.run(["gcc","k-means.c","csv_reader.c","-o","k-means.exe","-lm"])
    if build.returncode !=0:
        sys.exit("compilation failed")
    result = subprocess.run(["k-means.exe"])
    if result.returncode != 0:
        sys.exit("k-means failed, nothing to graph")
    data = process_file("output.txt")
    print(data)
    plot_data(data)

if __name__ == "__main__":
    main()