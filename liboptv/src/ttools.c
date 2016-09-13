/****************************************************************************

Author/Copyright:      	Jochen Willneff

Address:	      	Institute of Geodesy and Photogrammetry
		       	ETH - Hoenggerberg
		       	CH - 8093 Zurich

Creation Date:	       	end of 99 ...going on

Description:	       	different search routines and track tools

Routines contained:     pix_in_next, candsearch_in_pix, searchposition,
                        predict, readseqtrackcrit, readtrackdata,
                        searchquader

****************************************************************************/
#include "ttools.h"




int candsearch_in_pix (target next[], int num, double x, double y, double dl,
    double dr, double du, double dd, int p[4], control_par *cpar) {
  /* search of four near candidates in targetlist */

  int  	  j, j0, dj, pnr = -999;
  int  zaehler=0, p1, p2, p3, p4;
  double  d, dmin=1e20, xmin, xmax, ymin, ymax;
  double d1, d2, d3, d4;
  xmin = x - dl;  xmax = x + dr;  ymin = y - du;  ymax = y + dd;

  if(xmin<0.0) xmin=0.0;
  if(xmax > cpar->imx)
        xmax = cpar->imx;
  if(ymin<0.0) ymin=0.0;
  if(ymax > cpar->imy)
    ymax = cpar->imy;

  if(x<0.0) x=0.0;
  if(x > cpar->imx)
    x = cpar->imx;
  if(y<0.0) y=0.0;
  if(y > cpar->imy)
    y = cpar->imy;

  p1 = p2 = p3 = p4 = -999;
  d1 = d2 = d3 = d4 = dmin;

  if (x >= 0.0 && x <= cpar->imx ) { if (y >= 0.0 && y <= cpar->imy ) {

  /* binarized search for start point of candidate search */
  for (j0=num/2, dj=num/4; dj>1; dj/=2)
    {
      if (next[j0].y < ymin) j0 += dj;
      else j0 -= dj;
    }

  j0 -= 12;  if (j0 < 0)  j0 = 0;	       	/* due to trunc */
  for (j=j0; j<num; j++)		       	/* candidate search */
    {
      if (next[j].tnr != -1 ) {
	if (next[j].y > ymax )  break;	       	/* finish search */
	if (next[j].x > xmin  &&  next[j].x < xmax && next[j].y > ymin  &&  next[j].y < ymax )
	  {
	    d = sqrt ((x-next[j].x)*(x-next[j].x) + (y-next[j].y)*(y-next[j].y));

	    if (d < dmin) { dmin = d; pnr = j;
	    }
	    if ( d < d1 )
	      {
		p4=p3; p3=p2; p2=p1; p1=j;
		d4=d3; d3=d2; d2=d1; d1=d;
	      }
	    else if ( d1 < d &&  d < d2 )
	      {
		p4=p3; p3=p2; p2=j;
		d4=d3; d3=d2; d2=d;
	      }
	    else if ( d2 < d && d < d3 )
	      {
		p4=p3; p3=j;
		d4=d3; d3=d;
	      }
	    else if ( d3 < d && d < d4 )
	      {
		p4=j;
		d4=d;
	      }
	  }
      }
    }

  p[0]=p1;
  p[1]=p2;
  p[2]=p3;
  p[3]=p4;
  for (j=0; j<4; j++) if ( p[j] != -999 ) {zaehler++; }
  } }
  return (zaehler);
}


int candsearch_in_pixrest(target  next[], int num, double x, double y,
    double dl, double dr, double du, double dd, int p[4], control_par *cpar) {
  /* search of four near candidates in targetlist */

  int  	  j, j0, dj;
  int  zaehler=0, p1, p2, p3, p4;
  double  d, dmin=1e20, xmin, xmax, ymin, ymax;
  xmin = x - dl;  xmax = x + dr;  ymin = y - du;  ymax = y + dd;

  if(xmin<0.0) xmin=0.0;
  if(xmax > cpar->imx) 
    xmax = cpar->imx;
  if(ymin<0.0) ymin=0.0;
  if(ymax > cpar->imy)
    ymax = cpar->imy;

  if(x<0.0) x=0.0;
  if(x > cpar->imx)
    x = cpar->imx;
  if(y<0.0) y=0.0;
  if(y > cpar->imy)
    y = cpar->imy;

  p1 = p2 = p3 = p4 = -999;

  /* binarized search for start point of candidate search */
  for (j0=num/2, dj=num/4; dj>1; dj/=2)
    {
      if (next[j0].y < ymin) j0 += dj;
      else j0 -= dj;
    }

  j0 -= 12;  if (j0 < 0)  j0 = 0;	       	/* due to trunc */
  for (j=j0; j<num; j++)		       	/* candidate search */
    {
      if (next[j].tnr == -1 ) {
	if (next[j].y > ymax )  break;	       	/* finish search */
	if (next[j].x > xmin  &&  next[j].x < xmax && next[j].y > ymin  &&  next[j].y < ymax )
	  {
	    d = sqrt ((x-next[j].x)*(x-next[j].x) + (y-next[j].y)*(y-next[j].y));
	    if (d < dmin) { dmin = d; p1 = j; }
	  }
      }
    }

  p[0]=p1;
  p[1]=p2;
  p[2]=p3;
  p[3]=p4;
  for (j=0; j<4; j++) if ( p[j] != -999 ) {zaehler++; }
  return (zaehler);
}


/* predict is used in display loop (only) of track.c to predict the position of 
   a particle in the next frame, using the previous and current positions
   Arguments: 
   vec2d a,b are vectors in 2D of the previous and current position, respectively
   vec2d c - output of the 2D positions of the particle in the next frame.
*/
void predict (vec2d a, vec2d b, vec2d c)
{
  c[0] = 2*b[0] - a[0];
  c[1] = 2*b[1] - a[1];
  return;
}

void searchquader(vec3d point, double xr[4], double xl[4], double yd[4], double yu[4], \
track_par *tpar, control_par *cpar, Calibration *glob_cal){
  int i, pt, dim;
  vec3d mins, maxes;
  double x, y, xz, yz;
  vec3d quader[8], pos;

  vec_set(mins, tpar->dvxmin, tpar->dvymin, tpar->dvzmin);
  vec_set(maxes, tpar->dvxmax, tpar->dvymax, tpar->dvzmax);
  /* 3D positions of search volume - eight corners of a box */
  for (pt = 0; pt < 8; pt++) {
    vec_copy(quader[pt], point);
    for (dim = 0; dim < 3; dim++) {
        if (pt & 1<<dim) {
            quader[pt][dim] += maxes[dim];
        } else {
            quader[pt][dim] += mins[dim];
        }
    }
  }

  /* calculation of search area in each camera */
  for (i = 0; i < cpar->num_cams; i++) {
      xr[i]=0;
      xl[i] = cpar->imx;
      yd[i]=0;
      yu[i] = cpar->imy;
      
      img_coord (point, &(glob_cal[i]), cpar->mm, &xz, &yz);
      metric_to_pixel (&xz, &yz, xz, yz, cpar);
      
      for (pt = 0; pt < 8; pt++) {
        img_coord (quader[pt], &(glob_cal[i]), cpar->mm, &x, &y);
	    metric_to_pixel (&x, &y, x, y, cpar);

	    if (x <xl[i] ) xl[i]=x;
	    if (y <yu[i] ) yu[i]=y;
	    if (x >xr[i] ) xr[i]=x;
	    if (y >yd[i] ) yd[i]=y;
	  }
      if (xl[i] < 0 ) xl[i]=0;
      if (yu[i] < 0 ) yu[i]=0;
      if (xr[i] > cpar->imx)
        xr[i] = cpar->imx;
      if (yd[i] > cpar->imy)
        yd[i] = cpar->imy;

      xr[i]=xr[i]-xz;
      xl[i]=xz-xl[i];
      yd[i]=yd[i]-yz;
      yu[i]=yz-yu[i];
  }
}



void sortwhatfound (foundpix item[16], int *zaehler, int num_cams)
{
  int i,j,m, different;
  foundpix temp;

  different=0;

  /* where what was found */
  for (i=0; i<16; i++)
    for (j=0; j<4; j++)
      for (m=0; m<4; m++)
	if(item[i].ftnr == item[4*j+m].ftnr)
	  {
	    item[i].whichcam[j]=1;
	  }

  /* how often was ftnr found */
  for (i=0; i<16; i++)
    for (j=0; j < num_cams; j++)
      if (item[i].whichcam[j] == 1 && item[i].ftnr !=-1) item[i].freq++;

  /* sort freq */
  for (i=1; i<16; ++i)  for (j=16-1; j>=i; --j)
    {
      if ( item[j-1].freq < item[j].freq )
	{
	  temp = *(item+j-1); *(item+j-1) = *(item+j); *(item+j) = temp;
	}
    }

  for (i=0; i<16; i++)
    for (j=i+1; j<16; j++)
      {
	if (item[i].ftnr == item[j].ftnr || item[j].freq <2)
	  {
	    item[j].freq=0;
	    item[j].ftnr=-1;
	  }
      }

  /* sort freq */
  for (i=1; i<16; ++i)  for (j=16-1; j>=i; --j)
    {
      if ( item[j-1].freq < item[j].freq )
	{
	  temp = *(item+j-1); *(item+j-1) = *(item+j); *(item+j) = temp;
	}
    }
  for (i=0; i<16; ++i) if(item[i].freq != 0) different++;
  *zaehler=different;

}
