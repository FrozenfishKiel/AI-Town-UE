// Fill out your copyright notice in the Description page of Project Settings.



#include "CallDeepSeekComponent.h"
#include "VaRestSubsystem.h"
#include "VaRestTypes.h"
#include "VaRestRequestJSON.h"

// Sets default values for this component's properties
UCallDeepSeekComponent::UCallDeepSeekComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UCallDeepSeekComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	
}

//允许用户输入并通知DeepSeek
void UCallDeepSeekComponent::CallDeepSeekAPI(const FString& UserInput)
{

	UVaRestSubsystem* VaRestSubsystem = GEngine->GetEngineSubsystem<UVaRestSubsystem>();
	UVaRestRequestJSON* Request = VaRestSubsystem->ConstructVaRestRequestExt(EVaRestRequestVerb::POST, EVaRestRequestContentType::json);

	if (!Request) return;

	Request->SetURL("https://api.deepseek.com/chat/completions");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetHeader("Authorization", "Bearer sk-ee5086c865224fa1a9592812f73c0488");

	
	if (!bIsSystemPromptAdded) {
		UVaRestJsonObject* MessageObjectSystem = NewObject<UVaRestJsonObject>();
		MessageObjectSystem->SetStringField(TEXT("role"), TEXT("system"));
		MessageObjectSystem->SetStringField(TEXT("content"), TEXT("你是一只猫娘"));

		ConversationHistory.Add(MessageObjectSystem);  //初始设定添加到历史记录

		bIsSystemPromptAdded = true;
	}

	
	
	// 创建单个消息对象
	UVaRestJsonObject* MessageObjectUser = NewObject<UVaRestJsonObject>();
	MessageObjectUser->SetStringField(TEXT("role"), TEXT("user"));
	MessageObjectUser->SetStringField(TEXT("content"), UserInput);  //根据用户输入的来改变
	ConversationHistory.Add(MessageObjectUser);  //初始设定添加到历史记录
	
	// 创建请求JSON对象
	UVaRestJsonObject* RequestBody = NewObject<UVaRestJsonObject>();
	RequestBody->SetStringField(TEXT("model"),TEXT("deepseek-chat"));
   
	// 设置数组到请求体
	RequestBody->SetObjectArrayField(TEXT("messages"), ConversationHistory);  //发送全部历史
	
	// 其他参数设置
	RequestBody->SetBoolField(TEXT("stream"), false); // 注意字段名应为小写
	RequestBody->SetNumberField(TEXT("temperature"), 0.7);
	RequestBody->SetNumberField(TEXT("max_tokens"), 2048);



	// 设置请求体
	Request->SetRequestObject(RequestBody);
	
	Request->OnRequestComplete.AddDynamic(this, &UCallDeepSeekComponent::OnResponseReceived);
	//Request->OnRequestFail.AddDynamic(this, &UCallDeepSeekComponent::OnRequestFailed);
	Request->ExecuteProcessRequest();
	
}

void UCallDeepSeekComponent::OnResponseReceived(UVaRestRequestJSON* InRequest)
{
	if (InRequest->GetResponseCode() == 200)
	{
		
		// 获取整个响应JSON对象
		UVaRestJsonObject* ResponseObject = InRequest->GetResponseObject();
        
		// 检查有效性
		if (!ResponseObject)
		{
			UE_LOG(LogTemp, Error, TEXT("ResponseObject is null!"));
			return;
		}
		// 获取choices数组
		TArray<UVaRestJsonObject*> ChoicesArray = ResponseObject->GetObjectArrayField(TEXT("choices"));
		
		// 检查数组有效性
		if (ChoicesArray.Num() == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("No choices in response!"));
			return;
		}

		// 获取第一个choice元素
		UVaRestJsonObject* FirstChoice = ChoicesArray[0];
		if (!FirstChoice)
		{
			UE_LOG(LogTemp, Error, TEXT("FirstChoice is null!"));
			return;
		}

		// 获取message对象
		UVaRestJsonObject* MessageObject = FirstChoice->GetObjectField(TEXT("message"));
		if (!MessageObject)
		{
			UE_LOG(LogTemp, Error, TEXT("MessageObject is null!"));
			return;
		}

		// 提取content字段
		FString Content = MessageObject->GetStringField(TEXT("content"));
		if (!Content.IsEmpty())
		{
			GEngine->AddOnScreenDebugMessage(-1, 120.f, FColor::Green, Content);

			UVaRestJsonObject* AssistantMessage = NewObject<UVaRestJsonObject>();
			AssistantMessage->SetStringField("role", "system");
			AssistantMessage->SetStringField("content", Content);

			ConversationHistory.Add(AssistantMessage);  //将回复记录添加到历史记录

			OnCallDeepSeekComplete.Broadcast(Content);
			
			// 在此处将Content传递给UI或其他逻辑
			
			TrimHistory();  //检查是否超出历史记录上限
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get content field!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Request failed! Code: %d"), InRequest->GetResponseCode());
	}
}

//安全添加AI回复
void UCallDeepSeekComponent::AddMessageToHistory(const FString& Role, const FString& Content)
{
	UVaRestJsonObject* AssistantMessage = NewObject<UVaRestJsonObject>();
	AssistantMessage->SetStringField("role", Role);
	AssistantMessage->SetStringField("content", Content);
	ConversationHistory.Add(AssistantMessage);
	TrimHistory();
}
//检查历史记录，防止内存爆炸
void UCallDeepSeekComponent::TrimHistory()
{
	if (ConversationHistory.Num() > Max_History)
	{
		ConversationHistory.RemoveAt(0,ConversationHistory.Num() - Max_History);
	}
}

void UCallDeepSeekComponent::ResetConversation()
{
	if (!ConversationHistory.IsEmpty())
	{
		ConversationHistory.Empty(); //清空数组
		bIsSystemPromptAdded = false;
	}
}



// Called every frame
void UCallDeepSeekComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

