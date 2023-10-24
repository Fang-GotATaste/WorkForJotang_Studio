import torch
import torch.nn as nn
import torch.optim as optim
import torchvision
import torchvision.transforms as transforms

# 定义数据预处理，将图像数据转换为PyTorch的Tensor，并进行归一化处理
transform = transforms.Compose([transforms.ToTensor(), transforms.Normalize((0.5,), (0.5,))])

# 加载MNIST数据集，如果本地没有该数据集则下载
trainset = torchvision.datasets.MNIST(root='./data', train=True, download=True, transform=transform)
trainloader = torch.utils.data.DataLoader(trainset, batch_size=64, shuffle=True)

testset = torchvision.datasets.MNIST(root='./data', train=False, download=True, transform=transform)
testloader = torch.utils.data.DataLoader(testset, batch_size=64, shuffle=False)

# 定义CNN模型
class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        # 定义第一个卷积层，输入通道为1（灰度图像），输出通道为32，卷积核大小为3x3
        self.conv1 = nn.Conv2d(1, 32, 3, 1)
        # 定义第二个卷积层，输入通道为32，输出通道为64，卷积核大小为3x3
        self.conv2 = nn.Conv2d(32, 64, 3, 1)
        # 定义第一个全连接层，输入节点数为64*5*5（通过两次最大池化层，图像大小变为5x5）
        self.fc1 = nn.Linear(64*5*5, 128)
        # 定义输出层，输出节点数为10（对应10个数字类别）
        self.fc2 = nn.Linear(128, 10)

    def forward(self, x):
        # 使用ReLU激活函数进行非线性变换
        x = torch.relu(self.conv1(x))
        # 使用最大池化层进行下采样
        x = torch.max_pool2d(x, 2)
        x = torch.relu(self.conv2(x))
        x = torch.max_pool2d(x, 2)
        # 将多维数据压缩为一维，用于全连接层处理
        x = x.view(-1, 64*5*5)
        x = torch.relu(self.fc1(x))
        # 使用log_softmax进行多类别分类，得到预测结果的概率分布
        x = torch.log_softmax(self.fc2(x), dim=1)
        return x

# 初始化模型、损失函数和优化器
model = Net()
# 定义交叉熵损失函数，用于多类别分类问题
criterion = nn.CrossEntropyLoss()
# 使用Adam优化器进行模型参数的优化
optimizer = optim.Adam(model.parameters(), lr=0.001)

# 训练模型
epochs = 5
for epoch in range(epochs):
    running_loss = 0.0
    # 遍历训练数据集的每个batch
    for i, data in enumerate(trainloader, 0):
        # 获取输入数据和标签
        inputs, labels = data
        # 梯度置零，用于累积梯度
        optimizer.zero_grad()
        # 前向传播，计算预测值
        outputs = model(inputs)
        # 计算损失值
        loss = criterion(outputs, labels)
        # 反向传播，计算梯度
        loss.backward()
        # 更新模型参数
        optimizer.step()
        # 打印损失值
        running_loss += loss.item()
        if i % 100 == 99:  # 每100批次打印一次损失值
            print('[%d, %5d] loss: %.3f' % (epoch + 1, i + 1, running_loss / 100))
            running_loss = 0.0

print('Finished Training')

# 在测试集上测试模型
correct = 0
total = 0
# 在不需要梯度的上下文中进行模型的预测
with torch.no_grad():
    for data in testloader:
        inputs, labels = data
        # 获取模型的预测结果
        outputs = model(inputs)
        # 获取预测结果中概率最大的类别
        _, predicted = torch.max(outputs.data, 1)
        # 统计总数和预测正确的数量
        total += labels.size(0)
        correct += (predicted == labels).sum().item()

# 输出模型在测试集上的准确度
print('Accuracy on the test images: %d %%' % (100 * correct / total))