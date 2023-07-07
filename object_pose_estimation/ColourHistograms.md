# Histogram Color

| Method            | 2D Pose   | 3D Pose  | RPY   |
| -----------       | --------  | -------- | ----- |
| Colour_Histograms|  ✔️       | ✖️        | ✖️    


We can find the wiki here.
```
# wiki
https://github.com/Extended-Object-Detection-ROS/wiki_english/wiki/HistColor
```

Here, I'm using the code I used when I followed the **Construct ROS Perception in 5 Days** course. So to get started, We can try the following 

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
    <Attribute Name="HistColorCube_Red" Type="HistColor" Histogram="histograms/Cube_Red.yaml"/>
    <Attribute Name="Cube_Red_Size" Type="Size" MinAreaPc="0.00" MaxAreaPc="100"/>
    <Attribute Name="NotFractal" Type="Size" MinAreaPc="0.5" MaxAreaPc="100"/>
    
</AttributeLib>

<SimpleObjectBase>

    <SimpleObject Name="Cube_Red_hist" ID="1">
        <Attribute Type="Detect">HistColorCube_Red</Attribute>
        <Attribute Type="Check">Cube_Red_Size</Attribute>
<!--        <Attribute Type="Check">NotFractal</Attribute>-->
    </SimpleObject>

</SimpleObjectBase>


<RelationLib>



</RelationLib>


<ComplexObjectBase>


</ComplexObjectBase>

```
**Note**
- Here, you don't need the "Cube_Red_Size" and "NotFractal". We don't even need to use "NotFractal (Size)" since it gives an error in my code. That's the reason for the comment on that line. (The construct pople used it to filter out Fractal objects)
- We use the "Cube_Red_Size" to filter out larger and smaller objects

## Extract the Attribute 

In the above XML file, we don't have to manually enter the values for the **`"HistColorCube_Red"`** Attribute. We can use the node provided by the same package to find it out. 

For that, we need to create a place to save the Histogram yaml file and a launch file to launch the node 

```
roscd my_object_recognition_pkg/config/object_base_example
mkdir histograms

# And now we create histogram generator launch:
roscd my_object_recognition_pkg
touch launch/hist_color_params_collector_point.launch

```

In this launch file, we have to 
- Give a name to the yaml file - arg "hist_name"
- Path to save the yaml file - arg "hist_name"
- Remap the "image_raw"(default) to our rgb camera rostopic
 
**hist_color_params_collector_point.launch**
```
<launch>
    <arg name="output" value="screen"/>
    <arg name="hist_name" default="Cube_Red"/>
    <arg name="hist_name" default="$(find my_object_recognition_pkg)/config/object_base_example/histograms/$(arg hist_name).yaml"/>

    <node name="hist_color_params_collector_point_node" pkg="extended_object_detection" type="hist_color_params_collector_point_node" output="screen" required="true">
        <param name="out_filename" value="$(arg hist_path)"/>
        <remap from="image_raw" to="/head_mount_kinect2/rgb/image_raw"/>
    </node>
</launch>

```

Then after launching the file, it will open a GUI. In that
- LEFT click on the most iconic colour on the object. (turn read)
- WITHOUT MOVING THE CAMERA, RIGHT-click on the image. (turn green and This will fix the values and start detecting based on the histogram values.)
- Press the ESC key, and it will automatically save the ymal file in the specified location.
- it will also print out the Attribute values we need to put in our XML file in the same terminal

```
<Attribute Name="MyHistColorAttribute" Type="HistColor" Histogram="/home/user/rl_ws/src/my_object_recognition_pkg/config/object_base_example/histograms/Cube_Red.yaml"/>
<Attribute Name="MySizeAttribute" Type="Size" MinAreaPc="0.00" MaxAreaPc="100"/>
```

**Note**

Even though it shows the absolute path, we don't have to add that in the XML file. If we run the code in the same package

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

        <rosparam param="selectedOnStartSimple">[1]</rosparam>
        <rosparam param="selectedOnStartComplex">[-1]</rosparam>

        <rosparam param="selected_on_start_simple_objects">[1]</rosparam>
        <rosparam param="selected_on_start_complex_objects">[-1]</rosparam>

    </node>

</launch>

```


**Note:** 
- Here, we changed the "selected_on_start_simple_objects" and selectedOnStartSimple" to `[1]` so it will only print out the first ID pose. 
- If you want it to detect multiple, we can add an array like `[1,3,4]`. This will select only detect the objects that are with IDs 1,3 and 4.
- If you leave it empty `[]`, it will detect all the listed objects
- With `[-1]`, it will detect none
