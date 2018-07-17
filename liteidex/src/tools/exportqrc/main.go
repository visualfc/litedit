package main

import (
	"encoding/xml"
	"flag"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"io"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
)

var (
	root = "../../../"
)

var (
	flagRoot string
)

func init() {
	flag.StringVar(&flagRoot, "root", "", "setup liteide root")
}

func main() {
	flag.Parse()
	if flagRoot != "" {
		root = flagRoot
	}
	root, _ := filepath.Abs(root)
	log.Println("process qrc files in liteide root", root)
	var p Process
	p.ProcessDir(filepath.Join(root, "src/liteapp"))
	p.ProcessDir(filepath.Join(root, "src/plugins"))
	p.Export(filepath.Join(root, "deploy/liteapp/qrc/default"), CopyFile)
	p.Export(filepath.Join(root, "deploy/liteapp/qrc/gray"), MakeColorImage(GrayColor))
}

type Process struct {
	rccs []RCC
}

func (p *Process) ProcessDir(dir string) {
	filepath.Walk(dir, func(path string, info os.FileInfo, err error) error {
		if filepath.Ext(path) == ".qrc" {
			p.ProcessQrc(path)
		}
		return nil
	})
}

type QResource struct {
	Prefix string   `xml:"prefix,attr"`
	Files  []string `xml:"file"`
}

type RCC struct {
	Dir      string
	DirName  string
	FileName string
	XMLName  xml.Name  `xml:"RCC"`
	Resource QResource `xml:"qresource"`
}

func (rcc *RCC) IsEmtpy() bool {
	return len(rcc.Resource.Files) == 0
}

func (rcc *RCC) ImagesFiles() (images []string) {
	for _, file := range rcc.Resource.Files {
		if filepath.Ext(file) == ".png" {
			image := filepath.Join(rcc.Dir, file)
			_, err := os.Lstat(image)
			if err != nil {
				log.Println("warning, not find image", image)
			}
			images = append(images, image)
		}
	}
	return
}

func (p *Process) ProcessQrc(path string) error {
	data, err := ioutil.ReadFile(path)
	if err != nil {
		return err
	}
	var rcc RCC
	err = xml.Unmarshal(data, &rcc)
	if err != nil {
		log.Println(err)
		return err
	}
	rcc.Dir, rcc.FileName = filepath.Split(path)
	_, rcc.DirName = filepath.Split(filepath.Clean(rcc.Dir))
	p.rccs = append(p.rccs, rcc)
	return nil
}

func (p *Process) Export(outdir string, copyFn CopyFunc) error {
	log.Println("export", outdir)
	for _, rcc := range p.rccs {
		p.ExportQrc(outdir, rcc, copyFn)
	}
	return nil
}

func (p *Process) ExportQrc(outdir string, rcc RCC, copyFn CopyFunc) error {
	images := rcc.ImagesFiles()
	if len(images) == 0 {
		//log.Println("skip empty rcc", rcc.FileName)
		return nil
	}
	outpath := filepath.Join(outdir, rcc.DirName)
	os.MkdirAll(outpath, 0777)
	log.Println(rcc.DirName, "->", len(images))
	for _, file := range images {
		target := file[len(rcc.Dir):]
		dest := filepath.Join(outpath, target)
		d, _ := filepath.Split(dest)
		os.MkdirAll(d, 0777)
		err := copyFn(file, dest)
		if err != nil {
			log.Println(file, err)
		}
	}
	return nil
}

type CopyFunc func(string, string) error
type ColorFunc func(r, g, b uint8) (uint8, uint8, uint8)

func GrayColor(r uint8, g uint8, b uint8) (uint8, uint8, uint8) {
	Y, cb, cr := color.RGBToYCbCr(r, g, b)
	cb = 128
	cr = 128
	return color.YCbCrToRGB(Y, cb, cr)
}

func MakeColorImage(colorFn ColorFunc) func(string, string) error {
	return func(source string, dest string) error {
		return CopyImage(source, dest, colorFn)
	}
}

func CopyImage(source string, dest string, colorFn ColorFunc) error {
	f, err := os.Open(source)
	if err != nil {
		return err
	}
	srcImage, err := png.Decode(f)
	if err != nil {
		return err
	}
	dstImage := image.NewRGBA(srcImage.Bounds())
	draw.Draw(dstImage, dstImage.Bounds(), srcImage, srcImage.Bounds().Min, draw.Src)
	//	dstImage.
	b := dstImage.Bounds()
	for y := b.Min.Y; y < b.Max.Y; y++ {
		for x := b.Min.X; x < b.Max.X; x++ {
			c := dstImage.RGBAAt(x, y)
			r, g, b := colorFn(c.R, c.G, c.B)
			dstImage.SetRGBA(x, y, color.RGBA{r, g, b, c.A})
		}
	}

	w, err := os.Create(dest)
	if err != nil {
		return err
	}
	err = png.Encode(w, dstImage)
	if err != nil {
		return err
	}
	return nil
}

func CopyFile(source string, dest string) (err error) {
	sourcefile, err := os.Open(source)
	if err != nil {
		return err
	}
	defer sourcefile.Close()
	destfile, err := os.Create(dest)
	if err != nil {
		return err
	}
	defer destfile.Close()
	_, err = io.Copy(destfile, sourcefile)
	if err == nil {
		sourceinfo, err := os.Stat(source)
		if err != nil {
			err = os.Chmod(dest, sourceinfo.Mode())
		}
	}
	return
}
