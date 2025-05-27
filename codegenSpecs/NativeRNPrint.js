// @flow
import type { TurboModule } from 'react-native/Libraries/TurboModule/RCTExport';
import { TurboModuleRegistry } from 'react-native';

export type RNPrintOptions = {
  html?: string,
  filePath?: string,
  isLandscape?: boolean,
  jobName: string,
};

export interface Spec extends TurboModule {
  +print: (options: RNPrintOptions) => Promise<Object>;
}

export default (TurboModuleRegistry.get<Spec>('RNPrint'): ?Spec);
