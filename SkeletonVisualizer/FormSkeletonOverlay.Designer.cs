namespace wustl_mm.SkeletonVisualizer
{
    partial class FormSkeletonOverlay
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormSkeletonOverlay));
            this.pictureBoxImages = new System.Windows.Forms.PictureBox();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolStripButtonLoadImage = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonLoadSkeleton = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonSaveImage = new System.Windows.Forms.ToolStripButton();
            this.toolStripLabelZoom = new System.Windows.Forms.ToolStripLabel();
            this.toolStripComboBoxZoomLevel = new System.Windows.Forms.ToolStripComboBox();
            this.toolStripButtonInvert = new System.Windows.Forms.ToolStripButton();
            this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusLabelSkeletonFileName = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripButtonLoadDirections = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonClear = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonLoadEigens = new System.Windows.Forms.ToolStripButton();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxImages)).BeginInit();
            this.toolStrip1.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // pictureBoxImages
            // 
            this.pictureBoxImages.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBoxImages.Location = new System.Drawing.Point(0, 28);
            this.pictureBoxImages.Name = "pictureBoxImages";
            this.pictureBoxImages.Size = new System.Drawing.Size(688, 375);
            this.pictureBoxImages.TabIndex = 0;
            this.pictureBoxImages.TabStop = false;
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButtonLoadImage,
            this.toolStripButtonLoadSkeleton,
            this.toolStripButtonLoadDirections,
            this.toolStripButtonLoadEigens,
            this.toolStripButtonSaveImage,
            this.toolStripLabelZoom,
            this.toolStripComboBoxZoomLevel,
            this.toolStripButtonInvert,
            this.toolStripButtonClear});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(688, 25);
            this.toolStrip1.TabIndex = 1;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // toolStripButtonLoadImage
            // 
            this.toolStripButtonLoadImage.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripButtonLoadImage.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonLoadImage.Image")));
            this.toolStripButtonLoadImage.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonLoadImage.Name = "toolStripButtonLoadImage";
            this.toolStripButtonLoadImage.Size = new System.Drawing.Size(67, 22);
            this.toolStripButtonLoadImage.Text = "Load Image";
            this.toolStripButtonLoadImage.ToolTipText = "Load Image";
            this.toolStripButtonLoadImage.Click += new System.EventHandler(this.toolStripButtonLoadImage_Click);
            // 
            // toolStripButtonLoadSkeleton
            // 
            this.toolStripButtonLoadSkeleton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripButtonLoadSkeleton.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonLoadSkeleton.Image")));
            this.toolStripButtonLoadSkeleton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonLoadSkeleton.Name = "toolStripButtonLoadSkeleton";
            this.toolStripButtonLoadSkeleton.Size = new System.Drawing.Size(78, 22);
            this.toolStripButtonLoadSkeleton.Text = "Load Skeleton";
            this.toolStripButtonLoadSkeleton.Click += new System.EventHandler(this.toolStripButtonLoadSkeleton_Click);
            // 
            // toolStripButtonSaveImage
            // 
            this.toolStripButtonSaveImage.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripButtonSaveImage.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonSaveImage.Image")));
            this.toolStripButtonSaveImage.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonSaveImage.Name = "toolStripButtonSaveImage";
            this.toolStripButtonSaveImage.Size = new System.Drawing.Size(68, 22);
            this.toolStripButtonSaveImage.Text = "Save Image";
            this.toolStripButtonSaveImage.Click += new System.EventHandler(this.toolStripButtonSaveImage_Click);
            // 
            // toolStripLabelZoom
            // 
            this.toolStripLabelZoom.Name = "toolStripLabelZoom";
            this.toolStripLabelZoom.Size = new System.Drawing.Size(37, 22);
            this.toolStripLabelZoom.Text = "Zoom:";
            // 
            // toolStripComboBoxZoomLevel
            // 
            this.toolStripComboBoxZoomLevel.Items.AddRange(new object[] {
            "1x",
            "2x",
            "5x",
            "10x",
            "20x"});
            this.toolStripComboBoxZoomLevel.Name = "toolStripComboBoxZoomLevel";
            this.toolStripComboBoxZoomLevel.Size = new System.Drawing.Size(75, 25);
            this.toolStripComboBoxZoomLevel.Text = "10x";
            this.toolStripComboBoxZoomLevel.TextChanged += new System.EventHandler(this.toolStripComboBoxZoomLevel_TextChanged);
            // 
            // toolStripButtonInvert
            // 
            this.toolStripButtonInvert.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripButtonInvert.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonInvert.Image")));
            this.toolStripButtonInvert.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonInvert.Name = "toolStripButtonInvert";
            this.toolStripButtonInvert.Size = new System.Drawing.Size(71, 22);
            this.toolStripButtonInvert.Text = "InvertImage";
            this.toolStripButtonInvert.Click += new System.EventHandler(this.toolStripButtonInvert_Click);
            // 
            // openFileDialog
            // 
            this.openFileDialog.Filter = "BMP files|*.bmp|All files |*.*";
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabelSkeletonFileName});
            this.statusStrip1.Location = new System.Drawing.Point(0, 423);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(688, 22);
            this.statusStrip1.TabIndex = 2;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // toolStripStatusLabelSkeletonFileName
            // 
            this.toolStripStatusLabelSkeletonFileName.Name = "toolStripStatusLabelSkeletonFileName";
            this.toolStripStatusLabelSkeletonFileName.Size = new System.Drawing.Size(0, 17);
            // 
            // toolStripButtonLoadDirections
            // 
            this.toolStripButtonLoadDirections.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripButtonLoadDirections.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonLoadDirections.Image")));
            this.toolStripButtonLoadDirections.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonLoadDirections.Name = "toolStripButtonLoadDirections";
            this.toolStripButtonLoadDirections.Size = new System.Drawing.Size(98, 22);
            this.toolStripButtonLoadDirections.Text = "Load Direction File";
            this.toolStripButtonLoadDirections.Click += new System.EventHandler(this.toolStripButtonLoadDirections_Click);
            // 
            // toolStripButtonClear
            // 
            this.toolStripButtonClear.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripButtonClear.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonClear.Image")));
            this.toolStripButtonClear.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonClear.Name = "toolStripButtonClear";
            this.toolStripButtonClear.Size = new System.Drawing.Size(36, 22);
            this.toolStripButtonClear.Text = "Clear";
            this.toolStripButtonClear.Click += new System.EventHandler(this.toolStripButtonClear_Click);
            // 
            // toolStripButtonLoadEigens
            // 
            this.toolStripButtonLoadEigens.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripButtonLoadEigens.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonLoadEigens.Image")));
            this.toolStripButtonLoadEigens.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonLoadEigens.Name = "toolStripButtonLoadEigens";
            this.toolStripButtonLoadEigens.Size = new System.Drawing.Size(68, 22);
            this.toolStripButtonLoadEigens.Text = "Load Eigens";
            // 
            // FormSkeletonOverlay
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(688, 445);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.pictureBoxImages);
            this.Name = "FormSkeletonOverlay";
            this.Text = "Skeleton overlay tool";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxImages)).EndInit();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pictureBoxImages;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton toolStripButtonLoadImage;
        private System.Windows.Forms.ToolStripButton toolStripButtonLoadSkeleton;
        private System.Windows.Forms.ToolStripButton toolStripButtonSaveImage;
        private System.Windows.Forms.OpenFileDialog openFileDialog;
        private System.Windows.Forms.ToolStripComboBox toolStripComboBoxZoomLevel;
        private System.Windows.Forms.ToolStripLabel toolStripLabelZoom;
        private System.Windows.Forms.ToolStripButton toolStripButtonInvert;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabelSkeletonFileName;
        private System.Windows.Forms.ToolStripButton toolStripButtonLoadDirections;
        private System.Windows.Forms.ToolStripButton toolStripButtonLoadEigens;
        private System.Windows.Forms.ToolStripButton toolStripButtonClear;
    }
}