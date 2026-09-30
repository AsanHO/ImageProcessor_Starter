# ImageProcessor 과제 제출

## 구현 항목

- 
- 
- 
- 

## 실행 명령어 (예시)

<!-- 예시 입니다. 기존 예시를 지우고 구현한 항목에 맞게 명령어를 작성해주세요. -->

```powershell
# Grayscale 변환
.\x64\Release\ImageProcessor.exe --input .\Resource\1_astronaut.bmp --output .\Resource\1_astronaut_grayscale.bmp --filter grayscale
```

```powershell
# Blur 처리 + 임계값 지정
.\x64\Release\ImageProcessor.exe --input .\Resource\2_coffee.bmp --output .\Resource\2_coffee_blur_threshold.bmp --filter blur --threshold 128
```

```powershell
# 필터 파이프라인 체인(고급)
.\x64\Release\ImageProcessor.exe --input .\Resource\3_chelsea_cat.bmp --output .\Resource\3_chelsea_cat_pipeline.bmp --pipeline "grayscale, blur, threshold:128"
```
