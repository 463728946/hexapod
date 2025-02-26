using System;
using System.Collections.ObjectModel;
using System.Net;
using System.Net.Sockets;
using System.Windows;
using System.Windows.Input;

namespace ServoControlApp
{
    public partial class MainWindow
    {
        private bool startLog =false;
        
        public MainWindow()
        {
            InitializeComponent();
            this.DataContext = this;

            // 初始化数据
            InitializeServos();
           
        }
        
        // 左边的伺服控件集合
        public ObservableCollection<Servo> LeftServos { get; set; } = new ObservableCollection<Servo>();
        // 右边的伺服控件集合
        public ObservableCollection<Servo> RightServos { get; set; } = new ObservableCollection<Servo>();
        public ObservableCollection<LogEntry> LogEntries { get; set; } = new ObservableCollection<LogEntry>();

        private void InitializeServos()
        {
            for (var i = 0; i < 9; i++)
            {
                LeftServos.Add(new Servo { Order = i, Value = 350 });
                RightServos.Add(new Servo { Order = i, Value = 350 });
                //LogEntries.Add(new LogEntry { Timestamp = DateTime.Now, Message = $"Servo {i} initialized" });
            }
        }

        private void Connect_Click(object sender, RoutedEventArgs e)
        {
            // 这里可以添加连接验证逻辑
            startLog = true;
        }

        public void SendData(string message)
        {
            // 获取 IP 地址和端口
            var ipAddress = txtIpAddress.Text;
            if (!int.TryParse(txtPort.Text, out var port))
            {
                if (startLog)
                    LogEntries.Insert(0,new LogEntry { Timestamp = DateTime.Now, Message = "Invalid port number" });
                return;
            }

            // 使用 UDP 发送数据
            try
            {
                var client = new UdpClient();
                var data = System.Text.Encoding.ASCII.GetBytes(message);
                var endPoint = new IPEndPoint(IPAddress.Parse(ipAddress), port);
                client.Send(data, data.Length, endPoint);
                client.Close();
                if (startLog)
                    LogEntries.Insert(0,new LogEntry { Timestamp = DateTime.Now, Message = message});
            }
            catch (Exception ex)
            {
                if (startLog)
                    LogEntries.Insert(0,new LogEntry { Timestamp = DateTime.Now, Message = $"Failed to send data: {ex.Message}" });
            }
        }

        private void Control_OnMouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            LogEntries.Clear();
        }
    }

    // 伺服控件的数据模型
    public class Servo
    {
        public int Order { get; set; }
        public int Value { get; set; }
    }
    
    public class LogEntry
    {
        public DateTime Timestamp { get; set; }
        public string Message { get; set; }
    }
}