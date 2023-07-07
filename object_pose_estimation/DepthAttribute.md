# Depth

| Method            | 2D Pose   | 3D Pose  | RPY   |
| -----------       | --------  | -------- | ----- |
| Depth         |    ✔️     |   ✔️     |   ✖️  |


We can find the wiki here.
```
# wiki
https://github.com/Extended-Object-Detection-ROS/wiki_english/wiki/DepthAttribute
```

So like the previous ones, we cannot use this independently. 
We have to pair it up with different detect methods such as [Colour_Histograms](https://github.com/ncbdrck/MultiROS-Real/blob/main/ReactorX/object_pose_estimation/ColourHistograms.md) or [HSVColor](https://github.com/ncbdrck/MultiROS-Real/blob/main/ReactorX/object_pose_estimation/HSVColor.md) or the ones [available](https://github.com/Extended-Object-Detection-ROS/wiki_english/wiki/simpleobject_detection) in the [Extended-Object-Detection-ROS](https://github.com/Extended-Object-Detection-ROS) package.

We only use this to `Extract` **Depth** information from the detected image.


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

Then we have to change the XML file.
```
<?xml version="1.0" ?>

<AttributeLib>
    <Attribute Name="HSVColorCube_Red" Type="HSVColor" Hmin="0" Hmax="10" Smin="100" Smax="255" Vmin="100" Vmax="255"/>
    <Attribute Name="HSVCube_Red_Size" Type="Size" MinAreaPc="0.01" MaxAreaPc="20"/>
    
    <Attribute Name="Depth" Type="Depth" mode="3"/>
    
</AttributeLib>

<SimpleObjectBase>

     <SimpleObject Name="Cube_Red_depth" ID="3">
        <Attribute Type="Detect">HSVColorCube_Red</Attribute>
        <Attribute Type="Check">HSVCube_Red_Size</Attribute>
        <Attribute Type="Extract" Channel="DEPTH">Depth</Attribute>
    </SimpleObject>
</SimpleObjectBase>


<RelationLib>



</RelationLib>


<ComplexObjectBase>


</ComplexObjectBase>

```

## Extract - Depth info

If you look at the above code, we are using mode `3` since it gave me the best results.

Determines the distance to the object and calculates the translation vector in 3D space to the centre of the object. Modes:

    0 - Takes median in rect of the object.
    1 - As 0 but reduce rect twice.
    2 - Takes point in canter of a rect.
    3 - Uses the contour of the object to create a mask, inside of which the median is calculated.


## Detection

To detect the cube, we can create another launch file.
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

        <rosparam param="selectedOnStartSimple">[3]</rosparam>
        <rosparam param="selectedOnStartComplex">[-1]</rosparam>

        <rosparam param="selected_on_start_simple_objects">[3]</rosparam>
        <rosparam param="selected_on_start_complex_objects">[-1]</rosparam>

    </node>

</launch>

```

**Note:** 
- Here, we changed the "selected_on_start_simple_objects" and selectedOnStartSimple" to `[3]` so it will only print out the Third ID pose. 
- If you want it to detect multiple, we can add an array like `[1,3,4]`. This will select only detect the objects that are with IDs 1,3, and 4.
