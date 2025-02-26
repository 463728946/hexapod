using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace ServoControlApp
{
    public partial class ServoControl : UserControl
    {
        public ServoControl()
        {
            InitializeComponent();
        }

        // 定义依赖属性
        public static readonly DependencyProperty OrderProperty =
            DependencyProperty.Register("Order", typeof(int), typeof(ServoControl));

        public int Order
        {
            get { return (int)GetValue(OrderProperty); }
            set { SetValue(OrderProperty, value); }
        }

        public static readonly DependencyProperty ValueProperty =
            DependencyProperty.Register("Value", typeof(int), typeof(ServoControl));

        public int Value
        {
            get { return (int)GetValue(ValueProperty); }
            set { SetValue(ValueProperty, value); }
        }

        public string Side { get; set; }

        private void Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            // 更新 TextBlock 的值
            Value = Convert.ToInt32(((Slider)sender).Value);

            // 触发发送数据逻辑
            //
        }

        private void SendDataToServer()
        {
            var mainWindow = (MainWindow)Application.Current.MainWindow;
            mainWindow.SendData($"{Side},{Order},{Value}");
        }

        private void UIElement_OnPreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            SendDataToServer();
        }

        private void ButtonBase_OnClick(object sender, RoutedEventArgs e)
        {
            Slider.Value = 350;
            SendDataToServer();
        }
    }
}