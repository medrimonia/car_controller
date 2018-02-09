#include "traj_controller.h"

#define NB_POINTS 4

std::vector<float> TrajController::traj_vx    = {0.5, 0.0, -0.5, 0.0};
std::vector<float> TrajController::traj_vy    = {0.0, 0.0,  0.0, 0.0};
std::vector<float> TrajController::traj_omega = {0.0, 0.0,  0.0, 0.0};

TrajController::TrajController()
  : idx(0), period(1)
{
  if (traj_vx.size() != traj_vy.size() ||
      traj_vx.size() != traj_omega.size()) {
    throw std::logic_error("Size of trajectories is not consistent in TrajController");
  }
}


void TrajController::process(const cv::Mat & left_img,
                             const cv::Mat & right_img,
                             float * vx, float * vy, float * omega) {
  // Traj does not use images
  (void) left_img; (void) right_img;

  int point_id = idx / period;

  *vx = traj_vx[point_id];
  *vy = traj_vy[point_id];
  *omega = traj_omega[point_id];

  // Increase idx if last point has not been reached
  if (point_id < (int)(traj_vx.size() - 1)) {
    idx++;
  }
}
