#include <stdio.h>
#include "cJSON.h"

int main() {
    printf("Hello World");
    return 0;
}

// #include <iostream>
// #include <thread>
// #include <chrono>
// #include <string.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <windows.h>

// #define STD_HANDLE GetStdHandle(STD_OUTPUT_HANDLE)

// typedef enum layoutKind
// {
//     Text,
//     Button,
//     Container,
// 	PaddedElement
// };

// typedef struct layoutStyle
// {
//     int paddingX = 0;
//     int paddingY = 0;

// } layoutStyle;

// typedef struct layoutReference
// {
//     layoutKind type;
//     layoutStyle style;
//     layoutReference* parent;

// } layoutReference;

// typedef struct layoutText
// {
//     // Derive from layoutReference
// 	layoutKind type;
//     layoutStyle style;
//     layoutReference* parent;

// 	char* content;

// } layoutText;

// /// <summary>
// /// A button that can be selected and clicked to invoke specific actions.
// /// </summary>
// typedef struct layoutButton
// {
//     // Derive from layoutReference
//     layoutKind type;
//     layoutStyle style;
//     layoutReference* parent;

//     char* content;
//     bool isSelected;
//     bool isClicked;

// } layoutButton;

// typedef enum layoutContainerDirection
// {
// 	Horizontal,
// 	Vertical

// } layoutContainerDirection;

// /// <summary>
// /// A vertical or horizontal container that arranges its children in a single row or column.
// /// </summary>
// typedef struct layoutContainer
// {
//     // Derive from layoutReference
//     layoutKind type;
//     layoutStyle style;
//     layoutReference* parent;
//     layoutContainerDirection direction;

//     layoutReference** elements;
//     int elementCount;

//     int gap;

// } layoutContainer;

// static void layoutReset(layoutReference* layoutRef)
// {
//     layoutRef->parent = NULL;
//     layoutRef->style.paddingX = 0;
//     layoutRef->style.paddingY = 0;
// }

// static layoutText* createText(char* content)
// {
// 	layoutText* textPtr = (layoutText*)malloc(sizeof(layoutText));
//     layoutReset((layoutReference*)textPtr);
// 	textPtr->content = content;
// 	textPtr->type = layoutKind::Text;

// 	return textPtr;
// }

// static layoutButton* createButton(char* content, bool isSelected)
// {
// 	layoutButton* buttonPtr = (layoutButton*)malloc(sizeof(layoutButton));
//     layoutReset((layoutReference*)buttonPtr);
// 	buttonPtr->content = content;
// 	buttonPtr->isSelected = isSelected;
//     buttonPtr->isClicked = false;
//     buttonPtr->type = layoutKind::Button;

// 	return buttonPtr;
// }

// static layoutContainer* createContainer(layoutReference** elements, layoutContainerDirection direction, int elementCount, int gap = 1)
// {
//     layoutContainer* containerPtr = (layoutContainer*)malloc(sizeof(layoutContainer));
//     layoutReset((layoutReference*)containerPtr);
// 	containerPtr->elements = elements;
// 	containerPtr->direction = direction;
// 	containerPtr->elementCount = elementCount;
// 	containerPtr->gap = gap;
// 	containerPtr->type = layoutKind::Container;

//     for (int i = 0; i < elementCount; i++)
//     {
//         elements[i]->parent = (layoutReference*)containerPtr;
//     }
// 	return containerPtr;
// }

// /// <summary>
// /// Joins two styles together. Commonly seen combining inherited styles with element styles.
// /// </summary>
// static layoutStyle combineStyle(layoutStyle* styleA, layoutStyle* styleB)
// {
//     layoutStyle combinedStyle{};
//     combinedStyle.paddingX = styleA->paddingX + styleB->paddingX;
//     combinedStyle.paddingY = styleA->paddingY;
//     //combinedStyle.paddingY = styleA->paddingY + styleB->paddingY;

//     return combinedStyle;
// }

// static void printStyleY(layoutStyle* style)
// {
//     for (int i = 1; i <= style->paddingY; i++) std::cout << "\n";
// }
// static void printStyleX(layoutStyle* style)
// {
//     for (int i = 1; i <= style->paddingX; i++) std::cout << " ";
// }

// void print(layoutReference* layoutRef, CONSOLE_SCREEN_BUFFER_INFO* initialInfo, layoutStyle* inheritedStyle);

// void printButton(layoutButton* button, CONSOLE_SCREEN_BUFFER_INFO* initialInfo, layoutStyle* inheritedStyle = NULL)
// {
//     layoutStyle workingStyle = inheritedStyle == NULL ? button->style : combineStyle(&button->style, inheritedStyle);

// 	printStyleY(&workingStyle);
// 	printStyleX(&workingStyle);
//     if (button->isSelected)
//     {
//         SetConsoleTextAttribute(STD_HANDLE, BACKGROUND_INTENSITY);

//         printf(" %s ", button->content);

//         SetConsoleTextAttribute(STD_HANDLE, initialInfo->wAttributes);
//     }
//     else
//     {
//         printf("%s", button->content);
//     }
//     printStyleX(&workingStyle);
//     printStyleY(&workingStyle);
// }

// void printContainer(layoutContainer* container, CONSOLE_SCREEN_BUFFER_INFO* initialInfo, layoutStyle* inheritedStyle = NULL)
// {
// 	layoutStyle workingStyle = inheritedStyle == NULL ? container->style : combineStyle(&container->style, inheritedStyle);

// 	printStyleY(&workingStyle);
//     for (int i = 0; i < container->elementCount; i++)
//     {
//         //printStyleX(&container->style);

//         print(container->elements[i], initialInfo, &workingStyle);

//         if (i != container->elementCount - 1)
//         {
//             for (int i = 1; i <= container->gap + 1; i++)
//             {
//                 // TODO: Optimize this!            
//                 std::cout << (container->direction == layoutContainerDirection::Horizontal ? " " : "\n");
//             }
//         }
//     }
//     printStyleY(&workingStyle);
// }

// static void printPlainText(char* content, CONSOLE_SCREEN_BUFFER_INFO* initialInfo, layoutStyle* style = NULL)
// {
//     if (style != NULL)
//     {
//         printStyleY(style);
//         printStyleX(style);
//     }
//     printf("%s", content);
//     if (style != NULL)
//     {
//         printStyleX(style);
//         printStyleY(style);
//     }
// }

// void printText(layoutText* text, CONSOLE_SCREEN_BUFFER_INFO* initialInfo, layoutStyle* inheritedStyle = NULL)
// {
//     layoutStyle workingStyle = inheritedStyle == NULL ? text->style : combineStyle(&text->style, inheritedStyle);

// 	printPlainText(text->content, initialInfo, &workingStyle);
// }

// void print(layoutReference* layoutRef, CONSOLE_SCREEN_BUFFER_INFO* initialInfo, layoutStyle* inheritedStyle)
// {
//     if (layoutRef->type == Button)
//     {
//         printButton((layoutButton*)layoutRef, initialInfo, inheritedStyle);
//     }
//     else if (layoutRef->type == Container)
//     {
//         printContainer((layoutContainer*)layoutRef, initialInfo, inheritedStyle);
//     }
// 	else if (layoutRef->type == Text)
// 	{
//         printText((layoutText*)layoutRef, initialInfo, inheritedStyle);
// 	}
//     else
//     {
//         fprintf(stderr, "Layout reference has an invalid type!");
//         exit(1);
//     }
// }

// void printDebug(layoutReference* layoutRef, CONSOLE_SCREEN_BUFFER_INFO* initialInfo, layoutStyle* inheritedStyle)
// {
//     if (layoutRef->type == Button)
//     {
// 		printPlainText(((layoutButton*)layoutRef)->content, initialInfo, inheritedStyle);
//     }
//     else if (layoutRef->type == Container)
//     {
//         printPlainText((char*)"Container\n", initialInfo, inheritedStyle);
//     }
//     else if (layoutRef->type == Text)
//     {
//         printPlainText((char*)"Text\n", initialInfo, inheritedStyle);
//     }
//     else
//     {
//         fprintf(stderr, "Layout reference has an invalid type!\n");
//         exit(1);
//     }
// }

// typedef struct layoutNeighborhoringElements
// {
//     layoutReference* top;
//     layoutReference* right;
//     layoutReference* bottom;
//     layoutReference* left;

// } layoutNeighborhoringElements;

// layoutReference* layoutGetFirstSelectable(layoutReference* layoutRef)
// {
//     if (layoutRef->type == Container)
//     {
//         auto layoutContainerRef = (layoutContainer*)layoutRef;

//         for (int i = 0; i < layoutContainerRef->elementCount; i++)
//         {
//             auto selectable = layoutGetFirstSelectable(layoutContainerRef->elements[i]);
//             if (selectable != NULL)
//             {
//                 return selectable;
//             }
//         }
//     }
//     else if (layoutRef->type == Button)
//     {
//         return layoutRef;
//     }
//     else return NULL;
// }

// layoutNeighborhoringElements layoutGetNeighbors(layoutReference* layoutRef)
// {
//     if (layoutRef == NULL)
//     {
// 		fprintf(stderr, "Unable to get neighbors: layout reference is null!");
//         exit(1);
//     }

// 	layoutNeighborhoringElements neighbors {};

// 	if (layoutRef->parent == NULL)
// 	{
// 		return neighbors;
// 	}

//     if (layoutRef->parent->type == Container)
//     {
// 	    layoutContainer* parentContainer = (layoutContainer*)layoutRef->parent;

// 	    for (int i = 0; i < parentContainer->elementCount; i++)
// 	    {
// 		    if (parentContainer->elements[i] == layoutRef)
// 		    {
//                 if (i > 0)
//                 {
//                     neighbors.left = parentContainer->elements[i - 1];
//                 }
//                 if (i < parentContainer->elementCount - 1)
//                 {
//                     neighbors.right = parentContainer->elements[i + 1];
//                 }
// 		    }
// 	    }
//     }

//     auto parentParentContainer = (layoutContainer*)layoutRef->parent->parent;

//     if (neighbors.top == NULL
//         && parentParentContainer != NULL
//         && parentParentContainer->type == Container
//         && parentParentContainer->direction == Vertical)
//     {
//         // Attempt to go up!

//         neighbors.top = parentParentContainer->elements[parentParentContainer->elementCount - 1];
//     }

// 	return neighbors;
// }

// static void printNeighborElements(layoutNeighborhoringElements* neighbors, CONSOLE_SCREEN_BUFFER_INFO* initialInfo)
// {
//     printf("Top: ");
//     if (neighbors->top != NULL)
//     {
// 		printDebug(neighbors->top, initialInfo, NULL);
// 	}
// 	else
// 	{
// 		printf("NULL\n");
//     }

// 	printf("Right: ");
//     if (neighbors->right != NULL)
//     {
// 		printDebug(neighbors->right, initialInfo, NULL);
// 	}
// 	else
// 	{
// 		printf("NULL\n");
//     }

// 	printf("Bottom: ");
//     if (neighbors->bottom != NULL)
//     {
//         printDebug(neighbors->bottom, initialInfo, NULL);
//     }
//     else printf("NULL\n");

// 	printf("Left: ");
// 	if (neighbors->left != NULL)
// 	{
// 		printDebug(neighbors->left, initialInfo, NULL);
// 	}
// 	else printf("NULL\n");
// }

// static void clearConsole()
// {
// #ifdef _WIN32
//     system("cls");
// #else
//     system("clear");
// #endif
// }

// static void moveCursorToCoordinates(int x, int y) {

// #ifdef _WIN32
//     // Create a COORD structure and set the coordinates
//     COORD coord;
//     coord.X = x;
//     coord.Y = y;

//     // Set the console cursor position
//     SetConsoleCursorPosition(STD_HANDLE, coord);
// #else
//     // ANSI escape code to move the cursor
//     printf("\033[%d;%dH", y, x);
// #endif
// }

// int main()
// {
//     layoutButton* nextPageButton = createButton((char*)"Next Page", false);
//     layoutButton* lastPageButton = createButton((char*)"Last Page", false);
//     layoutButton* exitButton = createButton((char*)"Exit", false);

// 	void* controlButtons[] = { nextPageButton, lastPageButton, exitButton };

//     void* printers[] = {
//         createButton((char*)"X1C Cinder Block\t\tPrinting", false),
//         createButton((char*)"X1C Kachow\t\tPrinting", false),
//         createButton((char*)"P1S Dikshant\t\tIdle", false),
//         createButton((char*)"X1E Ripley\t\tIdle", false),
//         createButton((char*)"X1E Spies-Girls 4\t\tIdle", false)
//     };

//     layoutReference* elements[] = {
// 		(layoutReference*)createText((char *)"PNW Additive Manufacturing Service"),
// 		(layoutReference*)createContainer((layoutReference**)printers, layoutContainerDirection::Vertical, 4, 0),
//         (layoutReference*)createContainer((layoutReference**)controlButtons, layoutContainerDirection::Horizontal, sizeof(controlButtons) / sizeof(layoutReference*), 1)
//     };
// 	layoutReference* rootContainer = (layoutReference*)createContainer(elements, layoutContainerDirection::Vertical, 3);

//     rootContainer->style.paddingX = 2;
//     rootContainer->style.paddingY = 1;

//     // Printing out.

//     CONSOLE_SCREEN_BUFFER_INFO initalColor;
//     BOOL success = GetConsoleScreenBufferInfo(STD_HANDLE, &initalColor);

//     layoutReference* cursorPosition = (layoutReference*)lastPageButton;

//     if (cursorPosition != NULL && cursorPosition->type == layoutKind::Button)
//     {
//         ((layoutButton*)cursorPosition)->isSelected = true;
//     }

//     print(rootContainer, &initalColor, NULL);

//     while (true)
//     {
//         auto doRight = GetKeyState(VK_RIGHT) & 0x8000;
//         auto doLeft = GetKeyState(VK_LEFT) & 0x8000;

//         if (!doRight && !doLeft) continue;

//         auto neighbors = layoutGetNeighbors(cursorPosition);

//         if (doRight && neighbors.right != NULL)
//         {
//             if (cursorPosition != NULL && cursorPosition->type == layoutKind::Button)
//             {
//                 ((layoutButton*)cursorPosition)->isSelected = false;
//             }
//             cursorPosition = neighbors.right;
            
//         }
//         else if (doLeft && neighbors.left != NULL)
//         {
//             if (cursorPosition != NULL && cursorPosition->type == layoutKind::Button)
//             {
//                 ((layoutButton*)cursorPosition)->isSelected = false;
//             }
//             cursorPosition = neighbors.left;
//         }
//         else continue;

//         clearConsole();

//         if (cursorPosition != NULL && cursorPosition->type == layoutKind::Button)
//         {
//             ((layoutButton*)cursorPosition)->isSelected = true;
//         }

//         print(rootContainer, &initalColor, NULL);

// 		neighbors = layoutGetNeighbors(cursorPosition);
//         std::cout << "\n";
// 		printNeighborElements(&neighbors, &initalColor);
//         std::cout << "\n";

//         std::this_thread::sleep_for(std::chrono::milliseconds(250));
//     }
//     return 0;
// }
