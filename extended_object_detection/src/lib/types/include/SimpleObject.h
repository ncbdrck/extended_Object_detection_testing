#ifndef _SIMPLE_OBJECT_
#define _SIMPLE_OBJECT_

#include "ExtendedObjectInfo.h"
#include "Attribute.h"
#include <utility>
#include "Filtering.h"

namespace eod{
    
    enum IdentifyMode {
        HARD,
        SOFT
    };
            
    /// <summary>
	/// Class SimpleObject
	/// </summary>
	class SimpleObject{
	public:
	    
        int ID;
        std::vector<ExtendedObjectInfo> objects;
        double Probability;
        std::string name;
        double iou_threshold_d;
        int totalWeight;
        bool identified;
        int ident_seq;
        IdentifyMode identify_mode;  
        MergingPolicy merging_policy;

        /// <summary>
        /// Default constructor
        /// </summary>		 
        SimpleObject();
        
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="name">Object name</param>	
        SimpleObject(std::string name);
        
        /// <summary>
        /// Calls one of two types Identification methods
        /// </summary>		 
        /// <param name="frame">Destination image</param>	
        /// <returns>List of identified objects</returns>
        virtual std::vector <ExtendedObjectInfo> Identify(const InfoImage& frame, const InfoImage& depth, int seq = 0 );
        
        /// <summary>
        /// Hard identification
        /// </summary>		 
        /// <param name="frame">Destination image</param>	
        /// <returns>List of identified objects</returns>
        virtual std::vector <ExtendedObjectInfo> IdentifyHard(const InfoImage& frame, const InfoImage& depth, int seq = 0 );
                            
        /// <summary>
        /// Soft identification
        /// </summary>		 
        /// <param name="frame">Destination image</param>	
        /// <returns>List of identified objects</returns>
        virtual std::vector<ExtendedObjectInfo> IdentifySoft(const InfoImage& frame, const InfoImage& depth, int seq = 0 );        
    
        /// <summary>
        /// Draws found objets
        /// </summary>		 
        /// <param name="image">Destination image to draw</param>
        /// <param name="col">Color of object's text and rect, default it is white</param>	
        virtual void draw(cv::Mat& image, cv::Scalar col = cv::Scalar(255, 255, 255));
        
        /// <summary>
        /// Draws found objets
        /// </summary>		 
        /// <param name="image">Destination image to draw</param>
        /// <param name="col">Color of object's text and rect, default it is white</param>	        
        
        void drawOne(cv::Mat& image, ExtendedObjectInfo* obj, cv::Scalar col = cv::Scalar(255,255,255));

        /// <summary>
        /// Prints info about object
        /// </summary>	
        void printInfo();
        std::string getInfoStr();

        //
        // SOME INSTRUMENTS:
        //
        void AddModeAttribute(AttributeMode, AttributeChannel, Attribute*);
        
        /// <summary>
        /// Function delets rects that are inside of others
        /// </summary>		 
        /// <returns>Updated list of  objects</returns>
        std::vector <ExtendedObjectInfo> clearInsiders();	  
        std::vector <ExtendedObjectInfo> clearInsidersCnt();        
        
        std::vector<EoiFilter*> filters;

	protected:      
        
        void defaultInit();
                
        std::vector<std::pair< std::pair<AttributeMode, AttributeChannel>, Attribute*> > mode_attributes;
        
        /// <summary>
        /// Transform rects and vectors from one camera parameters to another
        /// </summary>	
        void transform_eoi(std::vector<ExtendedObjectInfo> &rects, const InfoImage& src, const InfoImage& dst);

        unsigned int image_samples;
        int borderX, borderY;
        double borderPc;        
    };
    
}

#endif // _SIMPLE_OBJECT_
