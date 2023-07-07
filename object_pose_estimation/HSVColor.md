# HSV Color

| Method            | 2D Pose   | 3D Pose  | RPY   |
| -----------       | --------  | -------- | ----- |
| HSV Color         |    ✔️     |    ✖️     |   ✖️  |


We can find the wiki here.
```
# wiki
https://github.com/Extended-Object-Detection-ROS/wiki_english/wiki/HSVcolor
```

Here, I'm using the code I used when I followed the **Construct ROS Perception in 5 Days** course. So to get started, We can try the following 

**Note:** This the same code block as in the [Colour_Histograms](https://github.com/ncbdrck/MultiROS-Real/blob/main/ReactorX/object_pose_estimation/ColourHistograms.md) one.

```
# Create a package to put the code
cd ~/rl_ws/src
mkdir my_object_recognition_pkg
cd my_object_recognition_pkg
mkdir -p config/object_base_example
roscd my_object_recognition_pkg/config/object_base_example
touch Cube_Detect.xml
```

Then we have to change the XML file
```
<?xml version="1.0" ?>

<AttributeLib>
    <Attribute Name="HSVColorCube_Red" Type="HSVColor" Hmin="0" Hmax="10" Smin="100" Smax="255" Vmin="100" Vmax="255"/>
    <Attribute Name="HSVCube_Red_Size" Type="Size" MinAreaPc="0.01" MaxAreaPc="20"/>
    
</AttributeLib>

<SimpleObjectBase>

     <SimpleObject Name="Cube_Red_hsv" ID="2">
        <Attribute Type="Detect">HSVColorCube_Red</Attribute>
        <Attribute Type="Check">HSVCube_Red_Size</Attribute>
    </SimpleObject>

</SimpleObjectBase>


<RelationLib>



</RelationLib>


<ComplexObjectBase>


</ComplexObjectBase>

```

## Extract the Attribute 

Similar to the [Colour_Histograms](https://github.com/ncbdrck/MultiROS-Real/blob/main/ReactorX/object_pose_estimation/ColourHistograms.md), we don't have to manually enter the values for the **`"HSVColorCube_Red"`** Attribute. 

For that, we need to create a place to save the Histogram yaml file and a launch file to launch the node. 

```
roscd my_object_recognition_pkg
touch launch/hsv_color_params_collector.launch
```

In this launch file, we only have to
- Remap the "image_raw"(default) to our rgb camera rostopic
 
 
**hsv_color_params_collector.launch**
```
<launch>
    <arg name="output" value="screen"/>

    <node name="hsv_color_params_collector_node" pkg="extended_object_detection" type="hsv_color_params_collector_node" output="screen" required="true" >
        <remap from="image_raw" to="/head_mount_kinect2/rgb/image_raw"/>
    </node>

</launch>
```

Then after launching the file, it will open a GUI. In that
- select the HSV colour values and the size value. (until the object turns green, and this will fix the values and start detecting based on the HSV values.)
- Press ESC key, and it will automatically close and print out the Attribute values we need to put in our XML file in the same terminal

```
<Attribute Name="MyHSVColorAttribute" Type="HSVColor" Hmin="0" Hmax="0" Smin="0" Smax="0" Vmin="22" Vmax="61"/>
<Attribute Name="MySizeAttribute" Type="Size" MinAreaPc="0.01" MaxAreaPc="100"/>
```

## Detection

To detect the cube, we can create another launch file
```
roscd my_object_recognition_pkg
touch launch/cube_detection.launch
```

The code is different from the Construct one since the package they have is old. So I had to add these new parameters to get it working following the demos in 
```
extended_object_detection/launch/extended_object_detection_oakd_example.launch
# and 
extended_object_detection/launch/extended_object_detection_webcam_example.launch
# and
extended_object_detection/launch/extended_object_detection_kinekt_example.launch
```

```
<launch>
    <arg name="output" default="screen"/>
    <arg name="objectBasePath" default="$(find my_object_recognition_pkg)/config/object_base_example/Simple_Gun.xml"/>

    <node name="extended_object_detection" pkg="extended_object_detection" type="extended_object_detection_node" output="screen">

        <param name="object_base" value="$(arg objectBasePath)"/>

        <param name="videoProcessUpdateRate" value="5"/>
        <param name="screenOutput" value="false"/>
        <param name="publishImage" value="true"/>
        <param name="publishMarkers" value="true"/>
        <param name="subscribeDepth" value="true"/>
        <param name="maxContourPoints" value="-1"/>

        <param name="broadcast_tf" value="true"/>
        <param name="rate_limit_sec" value="0.1"/>
        <param name="allowed_lag_sec" value="0"/>
        <param name="publish_image_output" value="true"/>
        <param name="publish_markers" value="true"/>

        <remap from="camera/image_raw" to="/head_mount_kinect2/rgb/image_raw"/>
        <remap from="camera/info" to="/head_mount_kinect2/rgb/camera_info"/>
        <remap from="depth/image_raw" to="/head_mount_kinect2/depth/image_raw"/>
        <remap from="depth/info" to="/head_mount_kinect2/depth/camera_info"/>

        <rosparam param="rgb_image_topics">[/head_mount_kinect2/rgb/image_raw]</rosparam>
        <rosparam param="rgb_info_topics">[/head_mount_kinect2/rgb/camera_info]</rosparam>
        <rosparam param="depth_image_topics">[/head_mount_kinect2/depth/image_raw]</rosparam>
        <rosparam param="depth_info_topics">[/head_mount_kinect2/depth/camera_info]</rosparam>

        <rosparam param="selectedOnStartSimple">[2]</rosparam>
        <rosparam param="selectedOnStartComplex">[-1]</rosparam>

        <rosparam param="selected_on_start_simple_objects">[2]</rosparam>
        <rosparam param="selected_on_start_complex_objects">[-1]</rosparam>

    </node>

</launch>

```

**Note:** Here, we changed the "selected_on_start_simple_objects" and selectedOnStartSimple" to `[2]` so it will only print out the second ID pose. If you want it to detect multiple, we can add an array like `[1,3,4]`. This will select only detect the objects that are with IDs 1,3 and 4.
