// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VaRestRequestJSON.h"
#include "CallDeepSeekComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCallDeepSeekComplete, FString, OutPutMessage);
UCLASS( Blueprintable , meta=(BlueprintSpawnableComponent) )
class MASSAITEST_API UCallDeepSeekComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCallDeepSeekComponent();
	UFUNCTION(BlueprintCallable)
	void ResetConversation();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void CallDeepSeekAPI(const FString& UserInput);
	UFUNCTION()
	void OnResponseReceived(UVaRestRequestJSON* InRequest);
	UFUNCTION()
	void OnRequestFailed(UVaRestRequestJSON* Request);
	UFUNCTION()
	void TrimHistory();  //超过数量自动处理
	void AddMessageToHistory(const FString& Role , const FString& Content);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(BlueprintAssignable)
	FOnCallDeepSeekComplete OnCallDeepSeekComplete;
protected:

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category="DefaultConfig")
	int32 Max_History = 0; // 保留最近X轮对话(user + assistant为一轮)
	
	TArray<TObjectPtr<UVaRestJsonObject>> ConversationHistory;
	
	bool bIsSystemPromptAdded = false;
};
