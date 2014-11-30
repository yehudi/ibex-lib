#include "ibex_CtcRasterPicture.h"

namespace ibex{

CtcRasterPicture::CtcRasterPicture(RasterPicture &data): Ctc(data.ndim), I(data)
{
    pixelCoords = new int[I.ndim];
}


//-------------------------------------------------------------------------------------------------------------
void CtcRasterPicture::worldToGrid(IntervalVector box, int *pixel_coord)
{
    for( int i = 0; i < box.size(); i++){
        box[i] = (box[i] - I.origin_[i]) / I.leaf_size_[i];
        // Limit range to image size on pixel_coord
        box[i] &= Interval(0,I.grid_size_[i]);
    }
    
    for (int i = 0; i < I.ndim; i++){
        pixel_coord[2*i]   = floor(box[i].lb());
        pixel_coord[2*i+1] = ceil(box[i].ub()-1);
    }
}

//-------------------------------------------------------------------------------------------------------------
void CtcRasterPicture::gridToWorld(int* pixel_coord, IntervalVector &box)
{
    for( int i = 0; i < I.ndim; i++){
        box[i] &= Interval(pixel_coord[2*i], pixel_coord[2*i+1]+1) * I.leaf_size_[i] + I.origin_[i];     
        if(box[i].is_empty()){
            box.set_empty();
            return;
        }   
    }
}

//----------------------------------------------------------------------------------------------------------------
void CtcRasterPicture::contract(IntervalVector &box){

    assert(box.size() == I.ndim);
    if(box.is_empty()) return;

    // Convert world coordinates into pixel coordinates
    worldToGrid(box, pixelCoords);

    // Contractor the box
    if( I.ndim == 2){
        contract(pixelCoords[0],pixelCoords[1],pixelCoords[2],pixelCoords[3]);
    } else if ( I.ndim == 3){
        contract(pixelCoords[0],pixelCoords[1],pixelCoords[2],pixelCoords[3],pixelCoords[4],pixelCoords[5]);        
    }
    // Check the result
    if(pixelCoords[0] == -1) {
        box.set_empty();
        return;
    }

    // Convert pixel coordinates into world coordinates
    gridToWorld(pixelCoords,box);
}

//------------------------------------------------------------------------------
//psi contraction
void CtcRasterPicture::contract(int &cxmin, int &cxmax, int &cymin, int &cymax){

    //compute enclosed pixels on consecutive lines from all dimensions of the box

    cxmax = std::max(0,std::min(I.grid_size_[0]-1,cxmax));
    cxmin = std::min(I.grid_size_[0]-1,std::max(0,cxmin));
    cymax = std::max(0,std::min(I.grid_size_[1]-1,cymax));
    cymin = std::min(I.grid_size_[1]-1,std::max(0,cymin));


    if ( enclosed_pixels(cxmin,cxmax,cymin,cymax) == 0){
        cxmin = -1;
        return;
    }

    //right
    for (int i = cxmin; i <= cxmax; i++)
    {
        if(enclosed_pixels(cxmin,i,cymin,cymax)>0) {
            cxmin = i;
            break;
        }
    }
    //left
    for (int i = cxmax; i >= cxmin ; i--)
    {
        if(enclosed_pixels(i,cxmax,cymin,cymax)>0) {
            cxmax = i;
            break;
        }
    }
    //down
    for (int j = cymin; j <=cymax; j++)
    {
        if(enclosed_pixels(cxmin,cxmax,cymin,j)>0) {
            cymin = j;
            break;
        }
    }
    //up
    for (int j = cymax; j >= cymin ; j--)
    {
        if(enclosed_pixels(cxmin,cxmax,j,cymax)>0) {
            cymax = j;
            break;
        }
    }
}
//////////////////////////////////////////////////////////////////////

void CtcRasterPicture::contract(int &cxmin, int& cxmax, int& cymin,int& cymax, int& czmin, int& czmax){

    cxmax = std::max(0,std::min(I.grid_size_[0]-1,cxmax));
    cxmin = std::min(I.grid_size_[0]-1,std::max(0,cxmin));
    cymax = std::max(0,std::min(I.grid_size_[1]-1,cymax));
    cymin = std::min(I.grid_size_[1]-1,std::max(0,cymin));
    czmax = std::max(0, std::min(I.grid_size_[2]-1,czmax));
    czmin = std::min(I.grid_size_[2]-1, std::max(0,czmin));
    
    if ( enclosed_pixels(cxmin,cxmax,cymin,cymax,czmin,czmax) == 0){
        cxmin = -1;
        return;
    }

    //right
    for (int i = cxmin; i <= cxmax; i++)
    {
        if(enclosed_pixels(cxmin,i,cymin,cymax,czmin,czmax)>0) {
            cxmin = i;
            break;
        }
    }
    //left
    for (int i = cxmax; i >= cxmin ; i--)
    {
        if(enclosed_pixels(i,cxmax,cymin,cymax,czmin,czmax)>0) {
            cxmax = i;
            break;
        }
    }
    //down
    for (int j = cymin; j <=cymax; j++)
    {
        if(enclosed_pixels(cxmin,cxmax,cymin,j,czmin,czmax)>0) {
            cymin = j;
            break;
        }
    }
    //up
    for (int j = cymax; j >= cymin ; j--)
    {
        int res = enclosed_pixels(cxmin,cxmax,j,cymax,czmin,czmax);
        if(res>0) {
            cymax = j;
            break;
        }
    }
    //bottom
    for (int k = czmin; k <=czmax; k++)
    {
        if(enclosed_pixels(cxmin,cxmax,cymin,cymax,czmin,k)>0) {
            czmin = k;
            break;
        }
    }
    //top
    for (int k = czmax; k >= czmin ; k--)
    {
        int res = enclosed_pixels(cxmin,cxmax,cymin,cymax,k,czmax);
        if(res>0) {
            czmax = k;
            break;
        }
    }
}

unsigned int CtcRasterPicture::enclosed_pixels(int xmin,int xmax,int ymin,int ymax){

    int b1 = I(xmax,ymax);
    int b2 = I(xmax,ymin-1);
    int b3 = I(xmin-1,ymax);
    int b4 = I(xmin-1,ymin-1);
    return b1 - b2 - b3 + b4;
    
}

unsigned int CtcRasterPicture::enclosed_pixels(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax)
{
    unsigned int L8 = I(xmax, ymax,zmax);
    unsigned int L5 = I(xmin-1, ymin-1, zmax);
    unsigned int L6 = I(xmin-1, ymax,zmax) ;
    unsigned int L7 = I(xmax, ymin-1,zmax);
    unsigned int L4 = I(xmax, ymax,zmin-1);
    unsigned int L1 = I(xmin-1, ymin-1, zmin-1);
    unsigned int L2 = I(xmin-1, ymax,zmin-1);
    unsigned int L3 = I(xmax, ymin-1,zmin-1);
    unsigned int L = (L8 + L5 - L6  - L7) -(L4 + L1 - L2 - L3);
    return L;
}






} // end namespace ibex
