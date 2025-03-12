using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace ESP32MiniServo;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow
{
    private TcpClient _client;
    private NetworkStream _stream;

    public MainWindow()
    {
        InitializeComponent();
    }
    
    private async void BtnConnect_Click(object sender, RoutedEventArgs e)
    {
        if (!IPAddress.TryParse(txtIP.Text, out _))
        {
            Log("输入的 IP 地址格式不正确，请检查。");
            return;
        }

        if (!int.TryParse(txtPort.Text, out int port) || port < 0 || port > 65535)
        {
            Log("输入的端口号无效，请输入 0 到 65535 之间的整数。");
            return;
        }

        try
        {
            // 连接到服务器
            _client = new TcpClient();
            await _client.ConnectAsync(txtIP.Text, port);
            _stream = _client.GetStream();
            Log($"已连接到服务器: {txtIP.Text}:{port}");
        }
        catch (Exception ex)
        {
            Log($"连接失败: {ex.Message}");
            _client = null;
            _stream = null;
        }
    }
    
    private void BtnGenerate_Click(object sender, RoutedEventArgs e)
    {
        var leftOffsetTicks = new int[3, 3];
        var rightOffsetTicks = new int[3, 3];

        var allCustomControls = FindVisualChildren<CustomControl>(this);
        foreach (var control in allCustomControls)
        {
            if (control.Side == "Left")
            {
                var row = control.Index / 3;
                var col = control.Index % 3;
                leftOffsetTicks[row, col] = control.Offset;
            }
            else
            {
                var row = (8 - control.Index) / 3;
                var col = (8 - control.Index) % 3;
                rightOffsetTicks[row, col] = control.Offset;
            }
        }

        var leftData = $"int left_offset_ticks[3][3] = {{{{{leftOffsetTicks[0, 0]}, {leftOffsetTicks[0, 1]}, {leftOffsetTicks[0, 2]}}}, {{{leftOffsetTicks[1, 0]}, {leftOffsetTicks[1, 1]}, {leftOffsetTicks[1, 2]}}}, {{{leftOffsetTicks[2, 0]}, {leftOffsetTicks[2, 1]}, {leftOffsetTicks[2, 2]}}}}};";
        var rightData = $"int right_offset_ticks[3][3] = {{{{{rightOffsetTicks[0, 0]}, {rightOffsetTicks[0, 1]}, {rightOffsetTicks[0, 2]}}}, {{{rightOffsetTicks[1, 0]}, {rightOffsetTicks[1, 1]}, {rightOffsetTicks[1, 2]}}}, {{{rightOffsetTicks[2, 0]}, {rightOffsetTicks[2, 1]}, {rightOffsetTicks[2, 2]}}}}};";

        Log(leftData);
        Log(rightData);
    }

    public async void SendData(int side, int index, int value, int offset)
    {
        if (_client == null || !_client.Connected)
        {
            Log("未连接到服务器，请先连接。");
            return;
        }

        try
        {
            // 构造数据包
            var data = Encoding.ASCII.GetBytes($"{side},{index},{value},{offset}");
            await _stream.WriteAsync(data);
            Log($"发送数据: {side},{index},{value},{offset}");
        }
        catch (Exception ex)
        {
            Log($"发送数据失败: {ex.Message}");
            _client?.Close();
            _client = null;
            _stream = null;
        }
    }

    private void Log(string message)
    {
        lstLog.Items.Add(new TextBlock { Text = $"{DateTime.Now:HH:mm:ss} - {message}" });
        lstLog.ScrollIntoView(lstLog.Items[lstLog.Items.Count - 1]);
    }

    protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
    {
        base.OnClosing(e);
        if (_client != null)
        {
            _client.Close();
        }
    }

    private IEnumerable<T> FindVisualChildren<T>(DependencyObject depObj) where T : DependencyObject
    {
        if (depObj != null)
        {
            for (var i = 0; i < VisualTreeHelper.GetChildrenCount(depObj); i++)
            {
                var child = VisualTreeHelper.GetChild(depObj, i);
                if (child != null && child is T t)
                {
                    yield return t;
                }

                foreach (var childOfChild in FindVisualChildren<T>(child))
                {
                    yield return childOfChild;
                }
            }
        }
    }
}