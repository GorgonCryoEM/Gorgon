using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace wustl_mm.SkeletonVisualizer
{
    public partial class FormSkeletonOverlay : Form
    {
        #region fields
        Bitmap sourceImage = null;
        Bitmap skeletonImage = null;
        int zoomLevel = 10;
        int[,] neighbors4 = {{0,1}, {1,0}, {-1,0}, {0,-1}};
        int xSize = 0, ySize = 0;
        double[,,] skelDirection;
        Pen skeletonPen = new Pen(Color.Red);
        Pen directionPen = new Pen(Color.Red);

        #endregion
        

        public FormSkeletonOverlay()
        {
            InitializeComponent();
            pictureBoxImages.Paint += new PaintEventHandler(pictureBoxImages_Paint);
            skeletonPen.Width = 3;
            directionPen.Width = 3;

        }

        #region methods

        bool SkeletonHasValue(int x, int y)
        {
            Color col = skeletonImage.GetPixel(x, y);
            return (col.R > 0 || col.G > 0 || col.B > 0);
        }

        void DrawImage(Graphics g)
        {
            if (sourceImage != null)
            {
                g.DrawImage(sourceImage, new Rectangle(0, 0, sourceImage.Width * zoomLevel, sourceImage.Height * zoomLevel), new Rectangle(0, 0, sourceImage.Width, sourceImage.Height), GraphicsUnit.Pixel);
            }

            if (skeletonImage != null)
            {
                for (int x = 1; x < skeletonImage.Width - 1; x++)
                {
                    for (int y = 1; y < skeletonImage.Height - 1; y++)
                    {
                        if(SkeletonHasValue(x, y)) 
                        {
                            bool found = false;
                            for (int n = 0; n < 4; n++)
                            {
                                if (SkeletonHasValue(x + neighbors4[n, 0], y + neighbors4[n, 1]))
                                {
                                    found = true;
                                    g.DrawLine(skeletonPen, x * zoomLevel, y * zoomLevel, (x + neighbors4[n, 0]) * zoomLevel, (y + neighbors4[n, 1]) * zoomLevel);
                                }
                            }
                            if (!found)
                            {
                                g.DrawEllipse(skeletonPen, x * zoomLevel - 1, y * zoomLevel - 1, 2, 2);
                            }
                        }
                        
                    }
                }
            }

            for (int x = 0; x < xSize; x++)
            {
                for (int y = 0; y < ySize; y++)
                {
                    //if ((skelDirection[x, y, 0] != 0) && (skelDirection[x, y, 1] != 0))
                    {
                        g.DrawLine(directionPen, x * zoomLevel, (ySize - y) * zoomLevel, (x + (float)skelDirection[x, y, 0]) * zoomLevel, (ySize - y - (float)skelDirection[x, y, 1]) * zoomLevel);
                    }

                }
            }
        }

        #endregion

        #region Event Handlers
        void pictureBoxImages_Paint(object sender, PaintEventArgs e)
        {
            DrawImage(e.Graphics);
        }

        private void toolStripButtonLoadImage_Click(object sender, EventArgs e)
        {
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                Bitmap image = new Bitmap(openFileDialog.FileName);
                sourceImage = new Bitmap(image.Width, image.Height);
                for (int x = 0; x < image.Width; x++)
                {
                    for (int y = 0; y < image.Height; y++)
                    {
                        Color col = image.GetPixel(x, y);
                        sourceImage.SetPixel(x, y, col);
                    }
                }
                pictureBoxImages.Invalidate();
            }
        }
        private void toolStripComboBoxZoomLevel_TextChanged(object sender, EventArgs e)
        {
            if (toolStripComboBoxZoomLevel.Text == "1x")
            {
                zoomLevel = 1; 
            } 
            else if (toolStripComboBoxZoomLevel.Text == "2x") 
            {
                zoomLevel = 2;
            }
            else if (toolStripComboBoxZoomLevel.Text == "5x")
            {
                zoomLevel = 5;
            }
            else if (toolStripComboBoxZoomLevel.Text == "10x")
            {
                zoomLevel = 10;
            }
            else if (toolStripComboBoxZoomLevel.Text == "10x")
            {
                zoomLevel = 20;
            }
            pictureBoxImages.Invalidate();
        }
        #endregion

        private void toolStripButtonLoadSkeleton_Click(object sender, EventArgs e)
        {
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                skeletonImage = new Bitmap(openFileDialog.FileName);
                toolStripStatusLabelSkeletonFileName.Text = openFileDialog.FileName;
                pictureBoxImages.Invalidate();             
            }
        }

        private void toolStripButtonInvert_Click(object sender, EventArgs e)
        {
            for (int x = 0; x < sourceImage.Width; x++)
            {
                for (int y = 0; y < sourceImage.Height; y++)
                {
                    Color col = sourceImage.GetPixel(x, y);
                    Color col2 = Color.FromArgb(255 - col.R, 255 - col.G, 255 - col.B);
                    sourceImage.SetPixel(x, y, col2);
                }
            }
            pictureBoxImages.Invalidate();
        }

        private void toolStripButtonSaveImage_Click(object sender, EventArgs e)
        {
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                Bitmap saveImage = new Bitmap(sourceImage.Width * zoomLevel, sourceImage.Height * zoomLevel);
                Graphics g = Graphics.FromImage(saveImage);
                DrawImage(g);
                saveImage.Save(saveFileDialog1.FileName);
            }
        }

        private void toolStripButtonClear_Click(object sender, EventArgs e)
        {
            skeletonImage = null;
        }

        private void toolStripButtonLoadDirections_Click(object sender, EventArgs e)
        {
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                StreamReader s = File.OpenText(openFileDialog.FileName);
                xSize = int.Parse(s.ReadLine());
                ySize = int.Parse(s.ReadLine());
                skelDirection = new double[xSize, ySize, 2];

                for (int x = 0; x < xSize; x++)
                {
                    for (int y = 0; y < ySize; y++)
                    {
                        skelDirection[x, y, 0] = double.Parse(s.ReadLine());
                        skelDirection[x, y, 1] = double.Parse(s.ReadLine());
                    }
                }
                s.Close();
            }
        }
    }
}