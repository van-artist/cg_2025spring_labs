# Lab4:网格参数化

---

姓名：刘尚  
学号：202211010058  
代码仓库：[Github](https://github.com/van-artist/cg_2025spring_labs)

---

## 环境配置

### 克隆代码仓库

```shell
git clone https://github.com/facebookresearch/DensePose.git
cd DensePose
```

### 查看基本信息

<img src="./images/lab4/截屏2025-05-17 09.45.48.png" alt="alt text" style="max-width:600px;" />

### conda 环境配置

```shell
conda create -n cg_labs python=3.9
conda activate cg_labs
pip install -r requirements.txt
```

下载解压 CG_E4 并编译

```shell
unzip CG_E4.zip
cd CG_E3
unzip chumpy.zip
unzip DensePose.zip
unzip val2014_part.zip
mv chumpy ../
mv DensePose ../
mv val2014_part ../
cd ../chumpy
pip install .
```

<img src="./images/lab4/截屏2025-05-17 10.16.12.png" alt="alt text" style="max-width:600px;" />

### 运行测试

```shell
cd notebooks
python COCO-on-SMPL.py
```

<img src="./images/lab4/截屏2025-05-17 10.24.21.png" alt="alt text" style="max-width:600px;" />
<img src="./images/lab4/截屏2025-05-17 10.24.47.png" alt="alt text" style="max-width:600px;" />

```shell
python RCNN-Texture-Transfer.py
```

<img src="./images/lab4/截屏2025-05-17 10.27.33.png" alt="alt text" style="max-width:600px;" />

```shell
python RCNN-Visualize-Results.py
```

<img src="./images/lab4/截屏2025-05-17 10.28.51.png" alt="alt text" style="max-width:600px;" />

## 实验任务

- 任务 1：在“COCO-on-SMPL.py”的基础上，编写新函数”smpl_view_set_axis_hand“，调整模 型位置与观察角度，查看清晰的手部关键点；
- 任务 2：在”COCO-Visualize.py”的基础上，在 DensePose COCO 数据集中自选图像展示人体 关键点与 mask；
- 任务 3：在 ”COCO-Visualize.py”与“RCNN-Texture-Transfer.py“的基础上，调用 TransferTexture 函数，将任务 2 中自选图像的纹理替换为示例纹理。

## 实验实现

### 任务 1

首先编写 `smpl_view_set_axis_hand` 函数，调整模型视角并聚焦手部：  
`smpl_view_set_axis_hand`函数实现如下：

```python
def smpl_view_set_axis_hand(ax, azimuth=0):
    ax.view_init(elev=0, azim=azimuth)
    max_range = 0.08
    ax.set_xlim(0.3 - max_range, 0.3 + max_range)
    ax.set_ylim(0.1 - max_range, 0.1 + max_range)
    ax.set_zlim(-0.1 - max_range, -0.1 + max_range)
    ax.axis('off')
```

然后添加手部视角的子图，并修改 `add_subplot` 的参数（1 行 4 列）：

```python

# 原图像中标注的2D身体关键点，仅x、y坐标
# Visualize the image and collected points.
ax = fig.add_subplot(141)
ax.imshow(Demo['ICrop']) # 原始RGB图像
ax.scatter(Demo['x'],Demo['y'],11, np.arange(len(Demo['y'])) ) # 在图像中加入散点
plt.title('Points on the image')
ax.axis('off'),

# 与标注对应的标准SMPL模型（T-Pose）中的3D身体关键点
## Visualize the full body smpl male template model and collected points
ax = fig.add_subplot(142, projection='3d')
ax.scatter(Z,X,Y,s=0.02,c='k')
ax.scatter(collected_z,  collected_x,collected_y,s=25,  c=  np.arange(len(Demo['y']))    )
smpl_view_set_axis_full_body(ax)
plt.title('Points on the SMPL model')

# 对应的T-Pose标准SMPL模型中的面部关键点
## Now zoom into the face.
ax = fig.add_subplot(143, projection='3d')
ax.scatter(Z,X,Y,s=0.2,c='k')
ax.scatter(collected_z,  collected_x,collected_y,s=55,c=np.arange(len(Demo['y'])))
smpl_view_set_axis_face(ax)
plt.title('Points on the SMPL model')

ax = fig.add_subplot(144, projection='3d')
ax.scatter(Z, X, Y, s=0.2, c='k')  # 模型点
ax.scatter(collected_z, collected_x, collected_y, s=55, c=np.arange(len(Demo['y'])))  # 映射关键点
smpl_view_set_axis_hand(ax, azimuth=60)  # 可调角度
plt.title("Points on the SMPL model (hand)")
```

实验结果如下

<img src="./images/lab4/截屏2025-05-17 10.42.56.png" alt="alt text" style="max-width:600px;" />
<img src="./images/lab4/截屏2025-05-17 10.43.11.png" alt="alt text" style="max-width:600px;" />

### 任务 2

先直接根据文件名查看图片 id

所以我们可以选择`Selected_im=319534`的图片进行实验

```python
Selected_im = 57597
```

运行代码

```shell
python COCO-Visualize.py
```

<img src="./images/lab4/截屏2025-05-17 11.37.40.png" alt="alt text" style="max-width:600px;" />
<img src="./images/lab4/截屏2025-05-17 11.37.52.png" alt="alt text" style="max-width:600px;" />
<img src="./images/lab4/截屏2025-05-17 11.38.01.png" alt="alt text" style="max-width:600px;" />

### 任务 3

写一个新文件`main.py`作为合并后的代码，实现增量修改

```python
import cv2
import matplotlib.pyplot as plt
import numpy as np
import os
from pycocotools.coco import COCO
import pycocotools.mask as mask_util

# 选择COCO数据集路径
coco_folder = '../detectron/datasets/data/coco/'
dp_coco = COCO(coco_folder + '/annotations/densepose_coco_2014_minival.json')

# 获取图像ID
im_ids = dp_coco.getImgIds()
Selected_im = 57597  # 选定图像 ID

# 加载图像和标注
im = dp_coco.loadImgs(Selected_im)[0]
ann_ids = dp_coco.getAnnIds(imgIds=im['id'])
anns = dp_coco.loadAnns(ann_ids)

# 加载图像
im_name = os.path.join(coco_folder + 'val2014', im['file_name'])
I = cv2.imread(im_name)

# 获取密集姿态遮罩
def GetDensePoseMask(Polys):
    MaskGen = np.zeros([256, 256])
    for i in range(1, 15):
        if Polys[i - 1]:
            current_mask = mask_util.decode(Polys[i - 1])
            MaskGen[current_mask > 0] = i
    return MaskGen

# 2D图像可视化
I_vis = I.copy() / 2  # 调低原图的亮度

# 生成mask并融合
for ann in anns:
    bbr = np.array(ann['bbox']).astype(int)
    if 'dp_masks' in ann.keys():
        Mask = GetDensePoseMask(ann['dp_masks'])
        x1, y1, x2, y2 = bbr[0], bbr[1], bbr[0] + bbr[2], bbr[1] + bbr[3]
        x2 = min([x2, I.shape[1]])
        y2 = min([y2, I.shape[0]])

        MaskIm = cv2.resize(Mask, (int(x2 - x1), int(y2 - y1)), interpolation=cv2.INTER_NEAREST)
        MaskBool = np.tile((MaskIm == 0)[:, :, np.newaxis], [1, 1, 3])

        Mask_vis = cv2.applyColorMap((MaskIm * 15).astype(np.uint8), cv2.COLORMAP_PARULA)[:, :, :]
        Mask_vis[MaskBool] = I_vis[y1:y2, x1:x2, :][MaskBool]
        I_vis[y1:y2, x1:x2, :] = (I_vis[y1:y2, x1:x2, :] * 0.3 + Mask_vis * 0.7) / 255.0

plt.imshow(I_vis[:, :, ::-1])
plt.axis('off')
plt.show()

# 读取并加载纹理图像（纹理图集）
Tex_Atlas = cv2.imread('../DensePoseData/demo_data/texture_from_SURREAL.png')[:, :, ::-1] / 255.
TextureIm = np.zeros([24, 200, 200, 3])

# 将纹理图集拆分为24个部分
for i in range(4):
    for j in range(6):
        TextureIm[(6 * i + j), :, :, :] = Tex_Atlas[(200 * j):(200 * j + 200), (200 * i):(200 * i + 200), :]

# TransferTexture 函数：将纹理映射到图像
def TransferTexture(TextureIm, im, IUV):
    U = IUV[:, :, 1]
    V = IUV[:, :, 2]
    R_im = np.zeros(U.shape)
    G_im = np.zeros(U.shape)
    B_im = np.zeros(U.shape)

    for PartInd in range(1, 25):
        tex = TextureIm[PartInd - 1, :, :, :].squeeze()
        R = tex[:, :, 0]
        G = tex[:, :, 1]
        B = tex[:, :, 2]

        x, y = np.where(IUV[:, :, 0] == PartInd)
        u_current_points = U[x, y]
        v_current_points = V[x, y]

        r_current_points = R[((255 - v_current_points) * 199. / 255.).astype(int), (u_current_points * 199. / 255.).astype(int)] * 255
        g_current_points = G[((255 - v_current_points) * 199. / 255.).astype(int), (u_current_points * 199. / 255.).astype(int)] * 255
        b_current_points = B[((255 - v_current_points) * 199. / 255.).astype(int), (u_current_points * 199. / 255.).astype(int)] * 255

        R_im[IUV[:, :, 0] == PartInd] = r_current_points
        G_im[IUV[:, :, 0] == PartInd] = g_current_points
        B_im[IUV[:, :, 0] == PartInd] = b_current_points

    generated_image = np.concatenate((B_im[:, :, np.newaxis], G_im[:, :, np.newaxis], R_im[:, :, np.newaxis]), axis=2).astype(np.uint8)
    BG_MASK = generated_image == 0
    generated_image[BG_MASK] = im[BG_MASK]
    return generated_image

# 纹理替换后的图像
IUV = cv2.imread('../DensePoseData/demo_data/synth_UV_example.png')
im = np.zeros(IUV.shape)d
image = TransferTexture(TextureIm, im, IUV)

# 显示最终结果：原图 + 替换纹理后的图像
plt.imshow(np.hstack((IUV, image[:, :, ::-1])))
plt.axis('off')
plt.show()
```

<img src="./images/lab4/截屏2025-05-17 14.24.05.png" alt="alt text" style="max-width:600px;" />
<img src="./images/lab4/截屏2025-05-17 14.24.16.png" alt="alt text" style="max-width:600px;" />
