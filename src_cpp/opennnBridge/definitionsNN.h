#pragma once 

enum ModuleType {E_APPROXIMATION = 1, E_CLASSIFICATION = 2, E_FORECASTING = 3 , E_NCODER_DECODER = 4, E_CUSTOMNN = 5, E_AE = 6, E_AEC = 7};

enum ScalingMethods {E_ScalingMethods_NoScaling = 1 , E_ScalingMethods_MinimumMaximum = 2 , E_ScalingMethods_MeanStandardDeviation = 3 , E_ScalingMethods_StandardDeviation = 4 , E_ScalingMethods_Logarithm = 5};
   
enum ActivationFunction {E_AF_Threshold = 1, E_AF_SymmetricThreshold = 2 , E_AF_Logistic = 3 , E_AF_HyperbolicTangent = 4 ,
                         E_AF_Linear = 5 , E_AF_RectifiedLinear = 6 , E_AF_ExponentialLinear = 7 , E_AF_ScaledExponentialLinear = 8 ,
                         E_AF_SoftPlus = 9 , E_AF_SoftSign = 10 , E_AF_HardSigmoid = 11  };

enum PoolingMethod { E_Pooling_Method_NoPooling = 1 , E_Pooling_Method_MaxPooling = 2 , E_Pooling_Method_AveragePooling = 3 };

enum ProbabilisticGeneralFunctions { PAF_Binary , PAF_Logistic , PAF_Competitive , PAF_Softmax };

enum LayerType {E_LAYER_TYPE_DEFAULT = 0, E_LAYER_TYPE_SCALING = 1, E_LAYER_TYPE_CONVOLUTIONAL = 2 , E_LAYER_TYPE_PERCEPTRON = 3 , E_LAYER_TYPE_POOLING = 4 , E_LAYER_TYPE_PROBABILISTIC = 5 ,
                E_LAYER_TYPE_LSTM = 6 , E_LAYER_TYPE_RECURRENT = 7 , E_LAYER_TYPE_UNSCALING = 8 , E_LAYER_TYPE_BOUNDING = 9 };
    
enum LossMethod {E_LOSS_METHOD_SUM_SQUARED_ERROR = 1, E_LOSS_METHOD_MSE = 2 /* MSE - Mean Squared Error */, E_LOSS_METHOD_NSE = 3 /* NSE - Normalized Squared Error */, 
                         E_LOSS_METHOD_MINKOWSKI_ERROR = 4, E_LOSS_METHOD_WSE = 5 /* WSE - Weighted Squared Error */, E_LOSS_METHOD_CEE = 6 /* CEE - Cross Entropy Error */};

enum OptimizationMethod {E_OM_GRADIENT_DESCENT = 1, E_OM_CONJUGATE_GRADIENT = 2, E_OM_QUASI_NEWTON_METHOD = 3 ,
                         E_OM_LEVENBERG_MARQUARDT_ALGORITHM = 4, E_OM_STOCHASTIC_GRADIENT_DESCENT = 5 , E_OM_ADAPTIVE_MOMENT_ESTIMATION = 6 /*ADAM*/};

enum LearningRateAlgorithm {E_LRA_GOLDEN_SECTION = 1 /* LRA -Learning Rate Algorithm */,E_LRA_BRENT_METHOD = 2}; 

using Tensor1DPtr = std::shared_ptr<Tensor<Index,1>>;
using Tensor2DPtr = std::shared_ptr<Tensor<float,2>>;