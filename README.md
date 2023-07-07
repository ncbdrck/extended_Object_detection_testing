# Extended-Object-Detection-ROS 

## Intro

We are using the following repo to get the pose and rpy of the object.
```
# Repo
https://github.com/Extended-Object-Detection-ROS/extended_object_detection

# Wiki
https://github.com/Extended-Object-Detection-ROS/wiki_english/wiki
```

There are many methods for detecting objects, and all the available methods are listed here.
```
# For simple objects
https://github.com/Extended-Object-Detection-ROS/wiki_english/wiki/simpleobject_detection

# For complex objects
https://github.com/Extended-Object-Detection-ROS/wiki_english/wiki/complexobject_detection
```

**Note**

- In our code pick and place project, we can get by with simple object detection since we are trying to capture the pose and rpy of a cube.
- But for more complex tasks, we first identify each shape as a simple object and use them to create large objects. (e.g.: a dog holding a bone.) 
- We first detect the dog and bone as simple objects. Then using those two, we find if they make up the complex object)


## Getting started

There are several methods to do object recognition from this package. But all of it involved creating a .xml file like the following. For that, we need to find  Attributes to

- Detect - we need to find the Attributes such as HistColor, HSVColor, 
- Check -  we need to find the Attributes such as Size, Blob
- Extract -  we need to find the Attributes such as Depth


**XML for detection**

```
<?xml version="1.0" ?>

<AttributeLib>
    <Attribute Name="HistColorCube_Red" Type="HistColor" Histogram="histograms/Cube_Red.yaml"/>
    <Attribute Name="Cube_Red_Size" Type="Size" MinAreaPc="0.00" MaxAreaPc="100"/>
    <Attribute Name="NotFractal" Type="Size" MinAreaPc="0.5" MaxAreaPc="100"/>

    <Attribute Name="HSVColorCube_Red" Type="HSVColor" Hmin="0" Hmax="10" Smin="100" Smax="255" Vmin="100" Vmax="255"/>
    <Attribute Name="HSVCube_Red_Size" Type="Size" MinAreaPc="0.01" MaxAreaPc="20"/>

    <Attribute Name="Depth" Type="Depth" mode="3"/>

<!--    <Attribute Name="HaarGun" Type="HaarCascade" Cascade="gun_haar/classifier/cascade.xml"/>-->
<!--    <Attribute Name="MyBlobAttribute" Type="Blob" minThreshold="54" maxThreshold="125" blobColor="0" minArea="1500" minCircularity="0.03" minConvexity="0.64" minInertiaRatio="0.00"/>-->

</AttributeLib>

<SimpleObjectBase>

    <SimpleObject Name="Cube_Red_hist" ID="1">
        <Attribute Type="Detect">HistColorCube_Red</Attribute>
        <Attribute Type="Check">Cube_Red_Size</Attribute>
<!--        <Attribute Type="Check">NotFractal</Attribute>-->
    </SimpleObject>

    <SimpleObject Name="Cube_Red_hsv" ID="2">
        <Attribute Type="Detect">HSVColorCube_Red</Attribute>
<!--        <Attribute Type="Check">NotFractal</Attribute>-->
        <Attribute Type="Check">HSVCube_Red_Size</Attribute>
    </SimpleObject>

    <SimpleObject Name="Cube_Red_depth" ID="3">
        <Attribute Type="Detect">HSVColorCube_Red</Attribute>
<!--        <Attribute Type="Check">NotFractal</Attribute>-->
        <Attribute Type="Check">HSVCube_Red_Size</Attribute>
        <Attribute Type="Extract" Channel="DEPTH">Depth</Attribute>
    </SimpleObject>

</SimpleObjectBase>


<RelationLib>



</RelationLib>


<ComplexObjectBase>


</ComplexObjectBase>


```


**Note:** 
- Here, we changed the "selected_on_start_simple_objects" and selectedOnStartSimple" to `[3]` so it will only print out the third ID pose. Find the launch file [here](https://github.com/ncbdrck/extended_Object_detection_testing/blob/main/my_object_recognition_pkg/launch/gun_detection.launch)
- If you want it to detect multiple, we can add an array like `[1,3,4]`. This will select only detect the objects that are with IDs 1,3 and 4.
- If you leave it empty `[]`, it will detect all the listed objects
- With `[-1]`, it will detect none

## Methods I tried at the moment (13th APR 2023)

In the above code, we are using three methods to detect objects. I will update the code with more methods I try in the future.

| Method            | 2D Pose   | 3D Pose  | RPY   |
| -----------       | --------  | -------- | ----- |
| [Colour_Histograms](https://github.com/ncbdrck/extended_Object_detection_testing/blob/main/object_pose_estimation/ColourHistograms.md) |  ✔️       | ✖️     | ✖️    |
| [HSVColor](https://github.com/ncbdrck/extended_Object_detection_testing/blob/main/object_pose_estimation/HSVColor.md)          |  ✔️       | ✖️        | ✖️    |
| [Depth](https://github.com/ncbdrck/extended_Object_detection_testing/blob/main/object_pose_estimation/DepthAttribute.md)             |  ✔️       | ✔️        | ✖️    |

**Note:** All the pose information we are getting is with respect to the `camera` frame. So we need to use additional code to convert it into the `world` or `base_link` frame


## Sample code to get the pose with respect to the base or world frame

```
from tf2_ros import Buffer
from tf2_ros import TransformListener
from extended_object_detection.msg import SimpleObjectArray, BaseObject
from tf2_geometry_msgs import PoseStamped
from geometry_msgs.msg import Point

simple_object_detection_sub = rospy.Subscriber('/extended_object_detection/simple_objects',
                                                            SimpleObjectArray, simple_object_detection_callback)

tf_buffer = Buffer()
listener = TransformListener(tf_buffer)
to_frame = "rx200/base_link"

cube_pose_vision = Point()


def transform_between_frames(input_pose_point, input_frame, output_frame):
    """
    This function transforms a pose from one frame of reference to another using tf2

    Args:
    - input_pose_point: a Point() object representing the pose to be transformed
    - input_frame: a string representing the frame of reference of the input pose
    - output_frame: a string representing the desired frame of reference for the output pose

    Returns:
    - a Point() object representing the transformed pose
    """

    # Create a PoseStamped object with the input pose and frame of reference
    input_pose_stamped = PoseStamped()
    input_pose_stamped.pose.position = input_pose_point
    input_pose_stamped.header.frame_id = input_frame
    input_pose_stamped.header.stamp = rospy.Time.now()

    # Use the tf_buffer to transform the input pose to the desired output frame of reference
    output_pose = tf_buffer.transform(input_pose_stamped, output_frame, rospy.Duration(1))

    return output_pose.pose.position

def simple_object_detection_callback(msg):
    """
    Callback function for the extended object detection - simple_objects

    Args:
    - msg: a DetectedObjectArray message containing information about detected objects

    This function updates the cube_pose_vision var with the position of the detected cube with respect to the
    robot_base.

    Returns:
    - None
    """

    # Get the frame of reference of the camera
    frame_id = msg.header.frame_id
    # print("frame_id:", frame_id)

    # Get the number of detected objects
    total_detected_objects = len(msg.objects)  # This will be one since we are only set to detect one in the launch

    # Iterate through the detected object
    for i in range(total_detected_objects):
        # Get the object ID, name, and confidence score
        object_id = msg.objects[i].type_id  # We are only looking for item [3] in the xml and launch file
        object_name = msg.objects[i].type_name  # name is "Cube_Red_depth"
        object_confidence_score = msg.objects[i].score

        # Get the pose of the detected object
        object_pose = Pose()
        object_pose.position.x = msg.objects[i].transform.translation.x
        object_pose.position.y = msg.objects[i].transform.translation.y
        object_pose.position.z = msg.objects[i].transform.translation.z
        object_pose.orientation.x = msg.objects[i].transform.rotation.x
        object_pose.orientation.y = msg.objects[i].transform.rotation.y
        object_pose.orientation.z = msg.objects[i].transform.rotation.z
        object_pose.orientation.w = msg.objects[i].transform.rotation.w

        # not needed since we are detecting one attribute
        if object_name == "Cube_Red_depth":

            # Transform the pose of the detected object to the base frame of reference
            # Unfortunately, this algo doesn't detect the rpy. So we only used position
            cube_position_camera_frame = object_pose.position
            cube_position_wrt_base_frame = transform_between_frames(cube_position_camera_frame, frame_id,
                                                                         to_frame)

            # Update the cube_pose attribute with the transformed pose
            cube_pose_vision = cube_position_wrt_base_frame  # point()
            # print("pose:", cube_position_wrt_base_frame)

```
