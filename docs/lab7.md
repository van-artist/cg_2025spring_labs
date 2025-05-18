# Lab7

---

姓名：刘尚  
学号：202211010058  
代码仓库：[Github](https://github.com/van-artist/cg_2025spring_labs)

---

## 环境配置

- 系统环境：MacOS
- 编译器：clang 15.0
- 考虑系统特性，这里选择手动从源码编译构建`Pytorch3D`

### 添加新的 conda 虚拟环境

```bash
brew install cmake ninja python@3.9
python3.9 -m venv ~/envs/FitMesh
source ~/envs/FitMesh/bin/activate
pip install --upgrade pip
pip install torch==1.13.1 torchvision==0.14.1 torchaudio==0.13.1
pip install iopath matplotlib plotly jupyter notebook
```

### 克隆仓库并编译

```bash
git clone https://github.com/facebookresearch/pytorch3d.git
cd pytorch3d
pip install -e .
```
