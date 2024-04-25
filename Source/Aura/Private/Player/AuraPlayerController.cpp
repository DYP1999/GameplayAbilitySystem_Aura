// Hello DYP


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	
	
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);
	if(!CursorHit.bBlockingHit) return;

	LastActor  = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());//如果这个actor实现这个接口，则转换成这个actor指针,但是这个actor指针和这个actor实现的接口是共用一个指针的 C++的规定   
		
	if(LastActor == nullptr)
	{
		if(ThisActor != nullptr)
		{
			ThisActor->HighlightActor();
			
		}
		else
		{
			//nothing
		}
		
	}
	else//last actor is valid
	{
		if(ThisActor!=nullptr)
		{
			if(ThisActor == LastActor)
			{
				//nothing
			}
			else
			{
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			
			
		}
		else
		{
			LastActor->UnHighlightActor();
		}
	}

}



void AAuraPlayerController::BeginPlay()//如果有三个角色 这个函数会调用5次  服务器3次  客户端各一次
{
	Super::BeginPlay();
	check(AuraContext);

	//这里必须要加判断 经过测试在listen服务器下 对pawn0执行getlocalplayer会得到他的本地player 但是 如果对 pawn1 和pawn2 执行则会得到空指针导致报错
	//因为客户端上只有一个控制器因此会拿到自己的player不会报错
	if(GetLocalPlayer())
	{
		UEnhancedInputLocalPlayerSubsystem*  Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		check(Subsystem);
		Subsystem->AddMappingContext(AuraContext,0);	
	}

	

	

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
	
	
	
	
	
	
	
	
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//转换原有的inputcomponent组件类型为enhancedinputcomponent
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	//将moveaction与移动函数绑定，设置的mapp自动会绑定move action
	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);
	
	
		
	
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f,Rotation.Yaw,0.f);//Yaw是相对于z轴的旋转,


	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);//获取玩家输入中y的值，添加到向前的方向
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);//获取玩家输入中x方向的值，添加到左右的方向
		
	}
		
	
}

