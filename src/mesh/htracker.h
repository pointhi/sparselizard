// sparselizard - Copyright (C) see copyright file.
//
// See the LICENSE file for license information. Please report all
// bugs and problems to <alexandre.halbach at gmail.com>.


// This object manages the mesh h-adaptivity for all element types.

#ifndef HTRACKER_H
#define HTRACKER_H

#include <iostream>
#include <vector>
#include <string>
#include "element.h"

class htracker
{

    private:
        
        // Store in compressed format all split info needed:
        // 
        // 1. 1/0 for fullsplit/transition element (if 0 this element stops here)
        // 3. 00/01/10//11 for 0/1/2/undefined through-edge tetrahedron fullsplit (only for tetrahedra)
        // 4. 1/0 for fullsplit/transition of first subelement
        // 5. ...
        // 6. 1/0 for fullsplit/transition of second subelement
        // 7. ...
        //
        // Elements types are ordered from lowest type number to highest.
        //
        std::vector<bool> splitdata = {};
        
        int maxdepth = 0;
        int numleaves = 0;
        
        // Curvature order of the original elements:
        int originalcurvatureorder;
        // Number of elements in each type at the 0 depth:
        std::vector<int> originalcount = {};
        
        // Number of subelements in each fullsplit type:
        std::vector<int> numsubelems = {1,2,4,4,8,8,8,10};
        
        // Information needed by the leaf cursor:
        int cursorposition = -1;
        int currentdepth = -1;
        int curtypeorigcountindex = -1;
        std::vector<int> parenttypes = {};
        std::vector<int> indexesinclusters = {};
        
        // Reference coordinates in the original element of each transition element:
        std::vector<std::vector<double>> teorc = {};
        std::vector<std::vector<int>> transitionelemsleafnums = {};

        // Get the reference and real corner node coordinates of all elements after adaptation (no transition elements added).
        // 'oc' are the original element coordinates including the curvature nodes.
        void getadapted(std::vector<std::vector<double>>& oc, std::vector<std::vector<double>>& arc, std::vector<std::vector<double>>& ac);
        
    public:

        // Number of highest dimension elements for each type (provide a vector of length 8) and their curvature order.
        htracker(int curvatureorder, std::vector<int> numelemspertype);
        
        int countleaves(void);
        int getmaxdepth(void);
        
        // Place cursor before first leaf:
        void resetcursor(void);
        // Move cursor forward (crashes when exceeding number of leaves). Position might not be at a leaf.
        // The through-edge number needed for the split is returned (-1 if none or no split).
        int next(void);
        // Check if the cursor is at a leaf:
        bool isatleaf(void);
        // Count the number of splits used to reach the current position:
        int countsplits(void);
        // Get the element type number at current position:
        int gettype(void);
        // Get the element type number of the current parent (-1 if at depth 0):
        int getparenttype(void);
        // Get the index of the current element in the cluster:
        int getindexincluster(void);
        
        // Count the number of splits used to reach each leaf (modifies the cursor):
        void countsplits(std::vector<int>& numsplits);
        // Get the element type number of all leaves (modifies the cursor):
        void gettype(std::vector<int>& types);
        // For each original element 'numsons' gives the number of sons of each type (by blocks of 8).
        void countsons(std::vector<int>& numsons);
        // For each leaf get the original element number:
        void getoriginalelementnumber(std::vector<int>& oen);
        
        // Count the number of elements of each type after adaptation (modifies the cursor):
        std::vector<int> countintypes(void);
        
        // Update the operations to how they will actually be treated (this removes individual grouping requests):
        void fix(std::vector<int>& operations);
        
        // Group/keep/split (-1/0/1) the requested leaves. Leaves are grouped if all
        // leaves in the cluster are tagged for grouping and none is already split.
        // NEIGBOUR ELEMENTS CANNOT DIFFER BY MORE THAN ONE SPLIT LEVEL.
        // The argument vector must have a size equal to the number of leaves.
        void adapt(std::vector<int>& operations);    
        
        // Get the node coordinates 'ac' of all elements after adaptation based on the original elements 
        // coordinates 'oc' (including curvature nodes). The parent number of each transition element is 
        // provided in 'leafnums' after execution.
        void getadaptedcoordinates(std::vector<std::vector<double>>& oc, std::vector<std::vector<double>>& ac, std::vector<std::vector<int>>& leafnums, std::vector<double> noisethreshold);
    
        // Get the reference coordinate in the original element 'orc' corresponding to the reference coordinates in the transition elements 'rc'.
        // 'ad[t][i]' gives the first position in 'rc' where the ith transition element of type t is. 'oad[i]' does that for the first position
        // of the ith original element in 'orc'. 'ad' and 'oad' have a 1 longer size and their last value is the vec size.
        void inoriginal(std::vector<std::vector<int>>& ad, std::vector<std::vector<double>>& rc, std::vector<int>& oad, std::vector<double>& orc);
    
        // Reduce size for storage:
        void tostorage(void);
    
        // Print the tree:
        void print(void);
        
        // Get length of 'splitdata' vector:
        int countbits(void);
};

#endif

