/**
 * Copyright (c) 2014 Carnegie Mellon University. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following acknowledgments and disclaimers.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. The names �Carnegie Mellon University,� "SEI� and/or �Software
 *    Engineering Institute" shall not be used to endorse or promote products
 *    derived from this software without prior written permission. For written
 *    permission, please contact permission@sei.cmu.edu.
 * 
 * 4. Products derived from this software may not be called "SEI" nor may "SEI"
 *    appear in their names without prior written permission of
 *    permission@sei.cmu.edu.
 * 
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 * 
 *      This material is based upon work funded and supported by the Department
 *      of Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon
 *      University for the operation of the Software Engineering Institute, a
 *      federally funded research and development center. Any opinions,
 *      findings and conclusions or recommendations expressed in this material
 *      are those of the author(s) and do not necessarily reflect the views of
 *      the United States Department of Defense.
 * 
 *      NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 *      INSTITUTE MATERIAL IS FURNISHED ON AN �AS-IS� BASIS. CARNEGIE MELLON
 *      UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR
 *      IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF
 *      FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS
 *      OBTAINED FROM USE OF THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES
 *      NOT MAKE ANY WARRANTY OF ANY KIND WITH RESPECT TO FREEDOM FROM PATENT,
 *      TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 * 
 *      This material has been approved for public release and unlimited
 *      distribution.
 **/
#include "Random_Edge_Coverage.h"

gams::algorithms::Random_Edge_Coverage::Random_Edge_Coverage (
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  platforms::Base * platform,
  variables::Sensors * sensors,
  variables::Self * self)
  : Base (knowledge, platform, sensors, self), init_ (false)
{
  status_.init_vars (*knowledge, "rac");

  // get boudning polygon from madara
  num_edges_ = knowledge->get (".rec.num_edges").to_integer ();
  for (int i = 0; i < num_edges_; ++i)
  {
    utility::Position pos;
    char expression[20];
    sprintf(expression, ".rec.edge%d", i);
    pos.x = knowledge->get (std::string (expression) + ".x").to_double ();
    pos.y = knowledge->get (std::string (expression) + ".y").to_double ();
    pos.z = 0;
    vertices_.push_back(pos);
  }
}

gams::algorithms::Random_Edge_Coverage::~Random_Edge_Coverage ()
{
}

void
gams::algorithms::Random_Edge_Coverage::operator= (const Random_Edge_Coverage & rhs)
{
  if (this != &rhs)
  {
    this->platform_ = rhs.platform_;
    this->sensors_ = rhs.sensors_;
    this->self_ = rhs.self_;
    this->status_ = rhs.status_;
  }
}


int
gams::algorithms::Random_Edge_Coverage::analyze (void)
{
  this->platform_->get_sensors (sensor_names_);

  platform_->get_position (current_position_);

  current_position_.to_container (self_->device.location);

  return 0;
}


int
gams::algorithms::Random_Edge_Coverage::execute (void)
{
  platform_->move(next_position_);
  return 0;
}


int
gams::algorithms::Random_Edge_Coverage::plan (void)
{
  // generate new next position if necessary
  if (!init_ || current_position_.approximately_equal(next_position_, 0.25))
  {
    init_ = true;
    generate_new_position();
  }

  return 0;
}

void
gams::algorithms::Random_Edge_Coverage::generate_new_position ()
{
  // select new edge
  int target_edge = rand() % num_edges_;

  // get endpoints
  const utility::Position & pos_1 = vertices_[target_edge];
  const utility::Position & pos_2 = vertices_[(target_edge + 1) % num_edges_];

  // get random point on line
  double delta_y = pos_2.y - pos_1.y;
  double delta_x = pos_2.x - pos_1.x;
  if (delta_y == 0) // east/west line
  {
    next_position_.x = generate_random_number(pos_1.x, pos_2.x);
    next_position_.y = pos_1.y;
  }
  else if (delta_x == 0) // north/south line
  {
    next_position_.y = generate_random_number(pos_1.y, pos_2.y);
    next_position_.x = pos_1.x;
  }
  else // other arbitrary line
  {
    double slope = (pos_2.y - pos_1.y) / (pos_2.x - pos_1.x);
    next_position_.x = generate_random_number(pos_1.x, pos_2.x);
    next_position_.y = slope * (next_position_.x - pos_1.x) + pos_1.y;
  }

  // fill in altitude on waypoint
  next_position_.z = current_position_.z;
}

double
gams::algorithms::Random_Edge_Coverage::generate_random_number(
  const double & a, const double & b) const
{
  double min = a < b ? a : b;
  double max = a > b ? a : b;
  double range = max - min;
  int random = rand();
  return min + (((double)random) / RAND_MAX) * range;
}
