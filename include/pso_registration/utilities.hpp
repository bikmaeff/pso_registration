#ifndef INCLUDE_PSO_REGISTRATION_UTILITIES_HPP_
#define INCLUDE_PSO_REGISTRATION_UTILITIES_HPP_

#include <assert.h>
#include <Eigen/Core>
#include <Eigen/Sparse>

#include <algorithm>
#include <limits>
#include <vector>
#include <math.h>
#include "pcl/common/distances.h"
#include "pcl/kdtree/kdtree_flann.h"
#include "pcl/point_cloud.h"
#include "pcl/point_types.h"

namespace pso_registration {

using pcl::euclideanDistance;

inline double calculateMSE(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
                           pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2) {
  assert(cloud1->size() == cloud2->size());
  double mse = 0;
  for (int i = 0; i < cloud1->size(); i++) {
    mse += euclideanDistance(cloud1->at(i), cloud2->at(i));
  }
  mse /= cloud1->size();
  return mse;
}

inline double averageClosestDistance(
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2) {
  double avgDistance = 0;
  pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
  kdtree.setInputCloud(cloud2);

  for (std::size_t i = 0; i < cloud1->size(); i++) {
    std::vector<int> neighbours;
    std::vector<float> distances;
    neighbours.reserve(1);
    distances.reserve(1);
    kdtree.nearestKSearch(*cloud1, i, 1, neighbours, distances);
    avgDistance += distances[0];
  }
  avgDistance /= cloud1->size();
  return avgDistance;
}

inline double sumSquaredError(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
                              pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2) {
  double sum = 0;
  double median_distance;
  pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
  kdtree.setInputCloud(cloud2);
  for (std::size_t i = 0; i < cloud1->size(); i++) {
    std::vector<int> neighbours;
    std::vector<float> distances;
    neighbours.reserve(1);
    distances.reserve(1);
    kdtree.nearestKSearch(*cloud1, i, 1, neighbours, distances);
    sum += distances[0];
  }
  return sum;
}

inline double l1_distance(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
                              pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2) {
  double sum = 0;
  pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
  kdtree.setInputCloud(cloud2);
  for (std::size_t i = 0; i < cloud1->size(); i++) {
    std::vector<int> neighbours;
    std::vector<float> distances;
    neighbours.reserve(1);
    distances.reserve(1);
    kdtree.nearestKSearch(*cloud1, i, 1, neighbours, distances);
    sum += sqrt(distances[0]);
  }
  return sum;
}

inline double robustSumSquaredError(
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2) {
  double sum = 0;
  double median_distance;
  pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
  kdtree.setInputCloud(cloud2);
  std::vector<double> all_distances;
  for (std::size_t i = 0; i < cloud1->size(); i++) {
    std::vector<int> neighbours;
    std::vector<float> distances;
    neighbours.reserve(1);
    distances.reserve(1);
    kdtree.nearestKSearch(*cloud1, i, 1, neighbours, distances);
    all_distances.push_back(distances[0]);
  }

  std::sort(all_distances.begin(), all_distances.end());
  if (all_distances.size() % 2 != 0) {
    median_distance = all_distances[(all_distances.size() + 1) / 2];
  } else {
    median_distance = (all_distances[all_distances.size() / 2] +
                       all_distances[(all_distances.size() / 2) + 1]) /
                      2.0;
  }
  int num_filtered = 0;
  for (auto it = all_distances.begin(); it != all_distances.end(); it++) {
    if (*it <= median_distance * 3 && *it >= median_distance / 3) {
      sum += (*it);
      num_filtered++;
    }
  }
  if (num_filtered < 10) {
    return std::numeric_limits<double>::max();
  }
  return sum;
}

inline double robustSumSquaredError(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
                                    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2,
                                    double factor) {
  double sum = 0;
  double median_distance;
  pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
  kdtree.setInputCloud(cloud2);
  std::vector<double> all_distances;
  for (std::size_t i = 0; i < cloud1->size(); i++) {
    std::vector<int> neighbours;
    std::vector<float> distances;
    neighbours.reserve(1);
    distances.reserve(1);
    kdtree.nearestKSearch(*cloud1, i, 1, neighbours, distances);
    all_distances.push_back(distances[0]);
  }

  std::sort(all_distances.begin(), all_distances.end());
  if (all_distances.size() % 2 != 0) {
    median_distance = all_distances[(all_distances.size() + 1) / 2];
  } else {
    median_distance = (all_distances[all_distances.size() / 2] +
                       all_distances[(all_distances.size() / 2) + 1]) /
                      2.0;
  }
  int num_filtered = 0;
  for (auto it = all_distances.begin(); it != all_distances.end(); it++) {
    if (*it <= median_distance * factor && *it >= median_distance / factor) {
      sum += (*it);
      num_filtered++;
    }
  }
  if (num_filtered < 10) {
    return std::numeric_limits<double>::max();
  }
  return sum;
}

inline double robustAveragedSumSquaredError(
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2) {
  double sum = 0;
  double median_distance;
  pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
  kdtree.setInputCloud(cloud2);
  std::vector<double> all_distances;
  for (std::size_t i = 0; i < cloud1->size(); i++) {
    std::vector<int> neighbours;
    std::vector<float> distances;
    neighbours.reserve(1);
    distances.reserve(1);
    kdtree.nearestKSearch(*cloud1, i, 1, neighbours, distances);
    all_distances.push_back(distances[0]);
  }

  std::sort(all_distances.begin(), all_distances.end());
  if (all_distances.size() % 2 != 0) {
    median_distance = all_distances[(all_distances.size() + 1) / 2];
  } else {
    median_distance = (all_distances[all_distances.size() / 2] +
                       all_distances[(all_distances.size() / 2) + 1]) /
                      2.0;
  }
  int num_filtered = 0;
  for (auto it = all_distances.begin(); it != all_distances.end(); it++) {
    if (*it <= median_distance * 3 && *it >= median_distance / 3) {
      sum += (*it);
      num_filtered++;
    }
  }
  if (num_filtered < 10) {
    return std::numeric_limits<double>::max();
  }
  return sum / num_filtered;
}

inline double medianClosestDistance(
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2) {
  double median_distance = 0;
  pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
  kdtree.setInputCloud(cloud2);
  std::vector<float> distances;
  for (std::size_t i = 0; i < cloud1->size(); i++) {
    std::vector<int> neighbours;
    std::vector<float> dist;
    neighbours.reserve(1);
    distances.reserve(1);
    kdtree.nearestKSearch(*cloud1, i, 1, neighbours, dist);
    distances.push_back(dist[0]);
  }
  std::sort(distances.begin(), distances.end());
  if (distances.size() % 2 != 0) {
    median_distance = distances[(distances.size() + 1) / 2];
  } else {
    median_distance = (distances[distances.size() / 2] +
                       distances[(distances.size() / 2) + 1]) /
                      2.0;
  }
  return median_distance;
}

inline double robustMedianClosestDistance(
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2) {
  double median_distance = 0;
  pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
  kdtree.setInputCloud(cloud2);
  std::vector<float> distances;
  std::vector<float> filtered_distances;
  for (std::size_t i = 0; i < cloud1->size(); i++) {
    std::vector<int> neighbours;
    std::vector<float> dist;
    neighbours.reserve(1);
    distances.reserve(1);
    kdtree.nearestKSearch(*cloud1, i, 1, neighbours, dist);
    distances.push_back(dist[0]);
  }
  std::sort(distances.begin(), distances.end());
  if (distances.size() % 2 != 0) {
    median_distance = distances[(distances.size() + 1) / 2];
  } else {
    median_distance = (distances[distances.size() / 2] +
                       distances[(distances.size() / 2) + 1]) /
                      2.0;
  }
  for (auto it = distances.begin(); it != distances.end(); it++) {
    if (*it <= median_distance * 3 && *it >= median_distance / 3.0) {
      filtered_distances.push_back(*it);
    }
  }
  if (filtered_distances.size() % 2 != 0) {
    median_distance = filtered_distances[(filtered_distances.size() + 1) / 2];
  } else {
    median_distance =
        (filtered_distances[filtered_distances.size() / 2] +
         filtered_distances[(filtered_distances.size() / 2) + 1]) /
        2.0;
  }
  return median_distance / filtered_distances.size();
}

inline double medianDistance(std::vector<Eigen::Triplet<double>> tripletList) {
  double median_distance;
  std::sort(tripletList.begin(), tripletList.end(),
            [](Eigen::Triplet<double> x, Eigen::Triplet<double> y) {
              return x.value() < y.value();
            });
  if (tripletList.size() % 2 != 0) {
    median_distance = tripletList[(tripletList.size() + 1) / 2].value();
  } else {
    median_distance = (tripletList[tripletList.size() / 2].value() +
                       tripletList[(tripletList.size() / 2) + 1].value()) /
                      2.0;
  }
  return median_distance;
}

inline Eigen::Quaterniond euler2Quaternion(const double roll,
                                           const double pitch,
                                           const double yaw) {
  Eigen::AngleAxisd rollAngle(roll, Eigen::Vector3d::UnitX());
  Eigen::AngleAxisd pitchAngle(pitch, Eigen::Vector3d::UnitY());
  Eigen::AngleAxisd yawAngle(yaw, Eigen::Vector3d::UnitZ());

  Eigen::Quaterniond q = yawAngle * pitchAngle * rollAngle;
  return q;
}

}  // namespace pso_registration

#endif  // INCLUDE_PSO_REGISTRATION_UTILITIES_HPP_
