using System.Windows;
using System.Windows.Controls;

namespace ESP32MiniServo;

public partial class CustomControl : UserControl
{
    public int Index { get; set; }
    public string Side { get; set; }
    public int Offset { get; set; }
    
    public CustomControl()
    {
        InitializeComponent();
        txtIndex.DataContext = this;
    }
    
    private void BtnMIN_Click(object sender, RoutedEventArgs e)
    {
        SendData(125);
    }

    private void BtnMID_Click(object sender, RoutedEventArgs e)
    {
        SendData(375);
    }

    private void BtnMAX_Click(object sender, RoutedEventArgs e)
    {
        SendData(600);
    }
    
    private void SendData(int value)
    {
        var mainWindow = Application.Current.MainWindow as MainWindow;
        if (mainWindow != null)
        {
            mainWindow.SendData(Side == "Left" ? 0 : 1, Index, value, Offset);
        }
    }

    private void TxtOffset_TextChanged(object sender, TextChangedEventArgs e)
    {
        if (!int.TryParse(TxtOffset.Text, out var offset))
        {
            MessageBox.Show("请输入有效的整数。", "输入错误", MessageBoxButton.OK, MessageBoxImage.Error);
            return;
        }
        Offset = offset;
    }
}