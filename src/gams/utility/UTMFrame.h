/**
 * Copyright (c) 2015 Carnegie Mellon University. All Rights Reserved.
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
 * @file UTMFrame.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the base reference Frame class
 **/

#ifndef _GAMS_UTILITY_UTM_FRAME_H_
#define _GAMS_UTILITY_UTM_FRAME_H_

#ifdef GAMS_UTM

#include <gams/pose/UTMFrame.h>
#include <gams/utility/ReferenceFrame.h>

namespace gams
{
  namespace utility
  {

    /**
     * A reference frame type  which handles UTM (between 84/-84 latitude)
     * and UPS coordinates (otherwise). All math is provided by GeographicLib
     * UTMUPS class; this is a wrapper to fit into the GAMS coordinate system
     * architecture.
     *
     * Be sure to use the northing(), easting(), zone(), and hemi() accessors
     * in Location. The base x() and y() values (especially x()) are not
     * traditional UTM representations.
     *
     * Distance calculations with UTMFrame are done two ways:
     *   If the two Locations are in the same zone, a simple cartesian distance
     *   is performed. Otherwise, if this frame is a child to a GPSFrame, the
     *   Locations are converted to that frame, then distance measured according
     *   to the GPSFrame. If neither of the above apply, distance_to is NAN
     *
     * Note: UTM's notion of bearing is not the same as GPS; the y axis does not
     *       necessarily point in the north direction, especially for near-polar
     *       UPS coordinates. For proper transformation of poses, be sure to
     *       transform the entire pose as a single Pose object. Do not transform
     *       Locations and Orientations individually if bearing is important.
     *
     * Deprecated backwards compatibility aliases. Will be removed in v2
     **/
    typedef gams::pose::UTMFrame UTMFrame;
  }
}

#endif
#endif
