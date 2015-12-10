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
 * 3. The names "Carnegie Mellon University," "SEI" and/or "Software
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
 *      INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
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

/**
 * @file Follow.h
 * @author Anton Dukeman <anton.dukeman@gmail.com>
 *
 * This file contains the declaration of the Follow algorithm
 **/

#ifndef   _GAMS_ALGORITHMS_FOLLOW_H_
#define   _GAMS_ALGORITHMS_FOLLOW_H_

#include <queue>

#include "gams/algorithms/BaseAlgorithm.h"
#include "gams/variables/Sensor.h"
#include "gams/platforms/BasePlatform.h"
#include "gams/variables/AlgorithmStatus.h"
#include "gams/variables/Self.h"
#include "gams/utility/GPSPosition.h"
#include "gams/algorithms/AlgorithmFactory.h"

namespace gams
{
  namespace algorithms
  {
    /**
    * An algorithm for following a target
    **/
    class GAMSExport Follow : public BaseAlgorithm
    {
    public:
      /**
       * Constructor
       * @param  id         id of agent to follow
       * @param  delay      timesteps to delay until follow
       * @param  knowledge  the context containing variables and values
       * @param  platform   the underlying platform the algorithm will use
       * @param  sensors    map of sensor names to sensor information
       * @param  self       self-referencing variables
       **/
      Follow (
        const madara::knowledge::KnowledgeRecord& id,
        const madara::knowledge::KnowledgeRecord& delay,
        madara::knowledge::KnowledgeBase * knowledge = 0,
        platforms::BasePlatform * platform = 0,
        variables::Sensors * sensors = 0,
        variables::Self * self = 0);
      
      /**
       * Destructor
       **/
      ~Follow ();

      /**
       * Assignment operator
       * @param  rhs   values to copy
       **/
      void operator= (const Follow & rhs);
      
      /**
       * Analyzes environment, platform, or other information
       * @return bitmask status of the platform. @see Status.
       **/
      virtual int analyze (void);
      
      /**
       * Plans the next execution of the algorithm
       * @return bitmask status of the platform. @see Status.
       **/
      virtual int execute (void);

      /**
       * Plans the next execution of the algorithm
       * @return bitmask status of the platform. @see Status.
       **/
      virtual int plan (void);
      
    protected:
      /// location of agent to follow
      madara::knowledge::containers::NativeDoubleArray target_location_;

      /// type of movement being executed
      utility::GPSPosition next_position_;

      /// previous locations of target agent
      std::queue<utility::GPSPosition> previous_locations_;

      /// timesteps to delay/max size of queue
      size_t delay_;
    };

    /**
     * A factory class for creating Follow Algorithms
     **/
    class GAMSExport FollowFactory : public AlgorithmFactory
    {
    public:

      /**
       * Creates a Follow Algorithm.
       * @param   args      arg[0] = the target to follow
       *                    arg[1] = the time step delay
       * @param   knowledge the knowledge base to use
       * @param   platform  the platform. This will be set by the
       *                    controller in init_vars.
       * @param   sensors   the sensor info. This will be set by the
       *                    controller in init_vars.
       * @param   self      self-referencing variables. This will be
       *                    set by the controller in init_vars
       * @param   agents   the list of agents, which is dictated by
       *                    init_vars when a number of processes is set. This
       *                    will be set by the controller in init_vars
       **/
      virtual BaseAlgorithm * create (
        const madara::knowledge::KnowledgeVector & args,
        madara::knowledge::KnowledgeBase * knowledge,
        platforms::BasePlatform * platform,
        variables::Sensors * sensors,
        variables::Self * self,
        variables::Agents * agents);
    };
  }
}

#endif // _GAMS_ALGORITHMS_FOLLOW_H_