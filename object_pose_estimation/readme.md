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
- But for more complex tasks, we first identify each shape as a simple object and use them to create large objects. (e.g.: a dog holding a bone. 
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
- Here, we changed the "selected_on_start_simple_objects" and selectedOnStartSimple" to `[3]` so it will only print out the third ID pose. 
- If you want it to detect multiple, we can add an array like `[1,3,4]`. This will select only detect the objects that are with IDs 1,3 and 4.
- If you leave it empty `[]`, it will detect all the listed objects
- With `[-1]`, it will detect none

## Methods I tried at the moment (13th APR 2023)

In the above code, we are using three methods to detect objects. I will update the code with more methods I try in the future.

| Method            | 2D Pose   | 3D Pose  | RPY   |
| -----------       | --------  | -------- | ----- |
| [Colour_Histograms](https://github.com/ncbdrck/MultiROS-Real/blob/main/ReactorX/object_pose_estimation/ColourHistograms.md) |  ✔️       | ✖️     | ✖️    |
| [HSVColor](https://github.com/ncbdrck/MultiROS-Real/blob/main/ReactorX/object_pose_estimation/HSVColor.md)          |  ✔️       | ✖️        | ✖️    |
| [Depth](https://github.com/ncbdrck/MultiROS-Real/blob/main/ReactorX/object_pose_estimation/DepthAttribute.md)             |  ✔️       | ✔️        | ✖️    |

**Note:** All the pose information we are getting is with respect to the `camera` frame. So we need to use additional code to convert it into the `world` or `base_link` frame
